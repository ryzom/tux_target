/* Copyright, 2010 Tux Target
 * Copyright, 2003 Melting Pot
 *
 * This file is part of Tux Target.
 * Tux Target is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Tux Target is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Tux Target; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

//
// Includes
//

#include <nel/misc/types_nl.h>

#include <math.h>
#include <stdio.h>
#include <ctype.h>

#include <map>
#include <vector>

#include <nel/misc/log.h>
#include <nel/misc/debug.h>
#include <nel/misc/displayer.h>
#include <nel/misc/config_file.h>

#include <nel/net/service.h>
#include <nel/net/login_cookie.h>

#include "login_service.h"
#include "connection_client.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//


//
// Functions
//

static void refuseShard(TServiceId sid, const char *format, ...)
{
	string reason;
	NLMISC_CONVERT_VARGS(reason, format, NLMISC::MaxCStringSize);
	nlwarning(reason.c_str ());
	CMessage msgout("FAILED");
	msgout.serial(reason);
	CUnifiedNetwork::getInstance()->send(sid, msgout);
}

static void cbWSConnection(const std::string &serviceName, TServiceId sid, void *arg)
{
	TSockId from;
	CCallbackNetBase *cnb = CUnifiedNetwork::getInstance ()->getNetBase (sid, from);
	const CInetAddress &ia = cnb->hostAddress (from);

	nldebug("new potential shard: %s", ia.asString ().c_str ());

	//TODO  HACK to keep only the offical server
	//return;
	
	// if we accept external shard, don't need to check if address is valid
	if(IService::getInstance ()->ConfigFile.getVar("AcceptExternalShards").asInt () == 1)
		return;

	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery("select * from shard where WSAddr='"+ia.ipAddress()+"'", nbrow, row, result);
	if(!reason.empty()) return;

	if(nbrow == 0)
	{
		// if we are here, it s that the shard have not a valid wsaddr in the database
		// we can't accept unknown shard
		refuseShard (sid, "Bad shard identification, the shard (WSAddr %s (sid = %d)) is not in the database and can't be added", ia.ipAddress ().c_str (),sid);
		return;
	}
}

static void cbWSDisconnection (const std::string &serviceName, TServiceId sid, void *arg)
{
	TSockId from;
	CCallbackNetBase *cnb = CUnifiedNetwork::getInstance()->getNetBase(sid, from);
	const CInetAddress &ia = cnb->hostAddress (from);

	nldebug("shard disconnection: %s", ia.asString().c_str());

	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery("select * from shard where InternalId="+toString(sid), nbrow, row, result);
	if(!reason.empty()) return;

	if(nbrow != 1)
	{
		nlwarning("disconnection of a shard that have %hu entries online", nbrow);
		return;
	}

	// shard disconnected
	nlinfo("ShardId %s with IP '%s' is offline!", row[0], ia.asString().c_str());
	nlinfo("*** ShardId %3s NbPlayers %3s -> %3d", row[0], row[2], 0);

	sqlQuery("update shard set State='Offline', InternalId=0, NbPlayers=0 where ShardId="+string(row[0]), nbrow, row, result);

	// put users connected on this shard offline
	sqlQuery("update user set State='Offline', ShardId=-1, Cookie='' where ShardId="+string(row[0]), nbrow, row, result);
}

// 
static void cbWSIdentification (CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	TSockId from;
	CCallbackNetBase *cnb = CUnifiedNetwork::getInstance ()->getNetBase (sid, from);
	const CInetAddress &ia = cnb->hostAddress (from);

	sint32 shardId;
	msgin.serial(shardId);

	sint32 nbplayers;
	msgin.serial(nbplayers);

	nldebug("Shard identification, It says to be ShardId %d with %d players, let check that!", shardId, nbplayers);

	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;
	

	reason = sqlQuery("select * from shard where ShardId="+toString(shardId), nbrow, row, result);
	if(!reason.empty()) { refuseShard(sid, reason.c_str()); return; }

	if(nbrow == 0)
	{
		if(IService::getInstance ()->ConfigFile.getVar("AcceptExternalShards").asInt () == 1)
		{
			// we accept new shard, add it
			reason = sqlQuery("insert into shard (ShardId, WsAddr, State, Name) values ("+toString(shardId)+", '"+ia.ipAddress()+"', 'Online', '"+ia.ipAddress()+"')", nbrow, row, result);
			if(!reason.empty()) { refuseShard(sid, reason.c_str()); return; }
			nlinfo("The ShardId %d with ip '%s' was inserted in the database and is online!", shardId, ia.ipAddress().c_str ());
		}
		else
		{
			// can't accept new shard
			refuseShard(sid, "Bad shard identification, The shard %d is not in the database and can't be added", shardId);
		}
		return;
	}
	else if(nbrow == 1)
	{
		// check that the ip is ok
		CInetAddress iadb;
		iadb.setNameAndPort (row[1]);
		nlinfo ("check %s with %s (%s)", ia.ipAddress ().c_str(), iadb.ipAddress().c_str(), row[1]);
		if (ia.ipAddress() != iadb.ipAddress())
		{
			// good shard id but from a bad computer address
			refuseShard(sid, "Bad shard identification, ShardId %d should come from '%s' and come from '%s'", shardId, row[1], ia.ipAddress ().c_str ());
			return;
		}

		if(row[4] == string("Online"))
		{
			//TODO MTR We have to change the way LS does this, look at NeLNS standard.
			const std::vector<TServiceId> &sidlist = CUnifiedNetwork::getInstance()->getConnectionList();
			std::vector<TServiceId>::const_iterator itr = sidlist.begin();
			bool fSvc=false;
			while(itr != sidlist.end()) {
				TServiceId sid2 = (*itr);
				if(sid2.get() == (uint16)atoi(row[5])) {
					// the shard is already online, disconnect the old one and set the new one
					refuseShard(sid2, "A new shard connects with the same IP/ShardId, you were replaced");
					fSvc=true;
				}
			}
			if(!fSvc)
				nlwarning("Unable to find sid %d in unified network connection list!", (uint16)atoi(row[5]));
			
			// TODO MTR: figure out how to look up TServiceId by uint16
		}
		else
		{
			string query = "update shard set State='Online' where ShardId="+toString(shardId);
			sint ret = mysql_query (DatabaseConnection, query.c_str ());
			if (ret != 0)
			{
				refuseShard (sid, "mysql_query (%s) failed: %s", query.c_str (),  mysql_error(DatabaseConnection));
				return;
			}
		}

		CMysqlResult result2;
		MYSQL_ROW row2;
		sint32 nbrow2;
		reason = sqlQuery("update shard set State='Online', NbPlayers="+toString(nbplayers)+", InternalId="+toString(sid)+" where ShardId="+toString(shardId), nbrow2, row2, result2);
		if(!reason.empty()) { refuseShard(sid, reason.c_str()); return; }

		// ok, the shard is identified correctly
		nlinfo("ShardId %d name '%s' wsaddr '%s' with ip '%s' is online!", shardId, row[3], row[1], ia.ipAddress().c_str());

		for(sint32 i = 0; i < nbplayers; i++)
		{
			sint32 uid;
			msgin.serial(uid);
			sqlQuery("update user set State='Online', ShardId="+toString(shardId)+", Cookie='' where UId="+toString(uid), nbrow2, row2, result2);
		}

		return;
	}
	else
	{
		refuseShard(sid, "mysql problem, There's more than 1 shard with the shardId %d in the database", shardId);
		return;
	}
	
	nlstop;
}

static void cbWSClientConnected(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	//
	// S16: Receive "CC" message from WS
	//

	// a WS tells me that a player is connected or disconnected
	uint32 Id;
	uint8 con;
	msgin.serial (Id);
	msgin.serial (con);	// con=1 means a client is connected on the shard, 0 means a client disconnected

	if(con)
		nlinfo ("Received a validation that client %u is connected on the frontend", Id);
	else
		nlinfo ("Received a validation that client %u is disconnected on the frontend", Id);
	
	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery("select * from user where UId="+toString(Id), nbrow, row, result);
	if(!reason.empty()) return;

	if(nbrow == 0)
	{
		nlwarning ("Id %d doesn't exist", Id);
		Output->displayNL ("###: %3d UId doesn't exist", Id);
		return;
	}
	else if(nbrow > 1)
	{
		nlerror ("Id %d have more than one entry!!!", Id);
		return;
	}

	// row[4] = State
	if(con == 1 && string(row[4]) != string("Waiting"))
	{
		nlwarning ("Id %d is not waiting but is %s", Id, row[4]);
		Output->displayNL ("###: %3d User isn't waiting, his state is '%s'", Id, row[4]);
		return;
	}
	else if(con == 0 && string(row[4]) != string ("Online"))
	{
		nlwarning ("Id %d wasn't connected on a shard but is %s", Id, row[4]);
		Output->displayNL ("###: %3d User wasn't connected on a shard, his state is '%s'", Id, row[4]);
		return;
	}

	CMysqlResult result2;
	MYSQL_ROW row2;
	sint32 nbrow2;

	if (con == 1)
	{
		// new client on the shard

		reason = sqlQuery("update user set State='Online', Cookie='' where UId="+toString(Id), nbrow2, row2, result2);
		if(!reason.empty()) return;

		reason = sqlQuery("update shard set NbPlayers=NbPlayers+1 where ShardId="+string(row[3]), nbrow2, row2, result2);
		if(!reason.empty()) return;

		nldebug ("Id %d is connected on the shard", Id);
		Output->displayNL ("###: %3d User connected to the shard (%s)", Id, row[3]);
	}
	else
	{
		// client removed from the shard (true is for potential other client with the same id that wait for a connection)

		reason = sqlQuery("update user set State='Offline', ShardId=-1, Cookie='' where UId="+toString(Id), nbrow2, row2, result2);
		if(!reason.empty()) return;

		reason = sqlQuery("update shard set NbPlayers=NbPlayers-1 where ShardId="+string(row[3]), nbrow2, row2, result2);
		if(!reason.empty()) return;
			
		nldebug ("Id %d is disconnected from the shard", Id);
		Output->displayNL ("###: %3d User disconnected from the shard (%s)", Id, row[3]);
	}
}

static void cbWSShardChooseShard(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	//
	// S10: receive "SCS" message from WS
	//

	CMessage msgout("SCS");

	string reason;
	msgin.serial(reason);
	msgout.serial(reason);

	string cookie;
	msgin.serial(cookie);

	if (reason.empty())
	{
		msgout.serial(cookie);

		string addr;
		msgin.serial(addr);
		msgout.serial(addr);
	}

	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;
	reason = sqlQuery("select * from user where Cookie='"+cookie+"'", nbrow, row, result);
	if(!reason.empty()) return;
	if(nbrow != 1)
	{
		nlwarning("cookie %s is not associated to anybody", cookie.c_str());
		return;
	}

	CLoginCookie lc;
	lc.setFromString(cookie);
	sendToClient(msgout, (TSockId)lc.getUserAddr());
}

static void cbScoreUpdate(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;

	// find if the shard is valid
	reason = sqlQuery("select * from shard where InternalId="+toString(sid), nbrow, row, result);
	if(!reason.empty()) return;
	if(nbrow != 1)
	{
		nlwarning("nbrow is %d", nbrow);
		return;
	}

	sint32 shardId = atoi(row[0]);

	string levelname;
	sint32 nbplayers;
	float sessionduration;
	msgin.serial(levelname, nbplayers, sessionduration);

	uint32 date = CTime::getSecondsSince1970();

	sqlQuery("insert into session (Date, SId, LevelName, NbPlayers, Duration) values (NOW(), "+toString(shardId)+", '"+levelname+"', "+toString(nbplayers)+", "+toString(sessionduration)+")", nbrow, row, result);
	uint32 sessionId = (uint32)mysql_insert_id(DatabaseConnection);

	for(sint32 i = 0; i < nbplayers; i++)
	{
		sint32 uid, score;
		float duration;
		msgin.serial(uid, score, duration);

		// update total score
		sqlQuery("update user set Score=Score+"+toString(score)+" where UId="+toString(uid), nbrow, row, result);

		sqlQuery("insert into user_session (UId, SessionId, Score, Duration) values ("+toString(uid)+", "+toString(sessionId)+", "+toString(score)+", "+toString(duration)+")", nbrow, row, result);
	}
}

static void cbBanClient(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;


	string ip;
	string userName;
	string kickerName;
	uint32 duration;
	msgin.serial(ip, userName, kickerName, duration);

	reason = sqlQuery("select * from ban where Ip='"+ip+"';", nbrow, row, result);
	if(!reason.empty()) 
		return;

	if(nbrow == 0)
	{
		string request = toString("insert into ban (Ip,Date,Duration) values ('%s',NOW(),%d);",ip.c_str(),duration);
		sqlQuery(request, nbrow, row, result);
	}
	string request = toString("insert into moderator_log (Moderator,Target,Command,Date) values ('%s','%s','%s %d',NOW());",kickerName.c_str(),userName.c_str(),"ban",duration);
	sqlQuery(request, nbrow, row, result);
}

static void cbKickClient(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;


	string ip;
	string userName;
	string kickerName;
	string info;
	msgin.serial(ip, userName, kickerName, info);

	string request = toString("insert into moderator_log (Moderator,Target,Command,Date) values ('%s','%s','%s',NOW());",kickerName.c_str(),userName.c_str(),"kick");
	sqlQuery(request, nbrow, row, result);
}

static void cbReportClient(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	string reason;
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;


	string ip;
	string userName;
	string reporterName;
	string info;
	msgin.serial(ip, userName, reporterName, info);

	string request = toString("insert into report (Reporter,User,Info,Date) values ('%s','%s','%s',NOW());",reporterName.c_str(),userName.c_str(),info.c_str());
	sqlQuery(request, nbrow, row, result);
}

static const TUnifiedCallbackItem WSCallbackArray[] =
{
	{ "CC", cbWSClientConnected },
	{ "WS_IDENT", cbWSIdentification },
	{ "SCS", cbWSShardChooseShard },
	{ "SU", cbScoreUpdate },
	{ "BC", cbBanClient},
	{ "KC", cbKickClient},
	{ "RC", cbReportClient},
};


//
// Functions
//

void connectionWSInit()
{
	CUnifiedNetwork::getInstance ()->addCallbackArray(WSCallbackArray, sizeof(WSCallbackArray)/sizeof(WSCallbackArray[0]));
	
	CUnifiedNetwork::getInstance ()->setServiceUpCallback("MTS", cbWSConnection);
	CUnifiedNetwork::getInstance ()->setServiceDownCallback("MTS", cbWSDisconnection);
}

void connectionWSUpdate()
{
}

void connectionWSRelease()
{
}
