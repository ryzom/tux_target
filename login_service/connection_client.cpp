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
//#include <nel/net/net_manager.h>
#include <nel/net/login_cookie.h>

#include "login_service.h"

#ifdef NL_OS_UNIX
extern "C" char *crypt (const char *__key, const char *__salt);
#endif


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//

static CVariable<bool> AcceptNewUsers("variables","AcceptNewUsers", "1 if the server accepts new users ", true, 0, true);

static CCallbackServer *ClientsServer = 0;

static const uint32 EncryptedSize = 13;


//
// Functions
//

void sendToClient(CMessage &msgout, TSockId sockId)
{
	nlassert(ClientsServer != 0);
	ClientsServer->send(msgout, sockId);
}

// Get a number between 0 and 64, used by cryptPassword
static uint32 rand64()
{
	return (uint32) floor(64.0*(double)rand()/((double)RAND_MAX+1.0));
}

// Crypt a password
static string cryptPassword(const string &password)
{
#ifdef NL_OS_UNIX
	char Salt[3];
	static char SaltString[65] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

	Salt[0] = SaltString[rand64()];
	Salt[1] = SaltString[rand64()];
	Salt[2] = '\0';

	return string(crypt(password.c_str(), Salt));
#else
	return password;
#endif
}

// Check if a password is valid
static string checkPassword(const string &password, const string &encrypted)
{
	if(password.empty())
	{
		return "Bad password, it must not be empty";
	}

	if(password.size() > 20)
	{
		return "Bad password, it must have less than 20 characters";
	}

	for(uint i = 0; i < password.size(); i++)
	{
		// switch to lower case
		if(!nlisprint(password[i]) || password[i] == ' ' || password[i] == ',' || password[i] == '{' || password[i] == '}' || password[i] == ';' || password[i] == '/' || password[i] == '\\' || password[i] == '"' || isspace(password[i]))
		{
			return toString("Bad password, character '%c' is not allowed", password[i]);
		}
	}

#ifdef NL_OS_UNIX
	char Salt[3];

	if (encrypted.size() != EncryptedSize)
	{
		return toString("Bad password, the encrypted pass size is %d and should be %d", encrypted.size(), EncryptedSize);
	}

	Salt[0] = encrypted[0];
	Salt[1] = encrypted[1];
	Salt[2] = '\0';

	return (encrypted == crypt (password.c_str(), Salt))?"":"Bad password";
#else
	return (encrypted == password)?"":"Bad password";
#endif
}

static string checkLogin(const string &login)
{
	if(login.empty())
	{
		return "Bad login, it must not be empty";
	}
	
	if(login.size() > 20)
	{
		return "Bad login, it must have less than 20 characters";
	}
	
//	for(uint i = 0; i < login.size(); i++)
//	{
//		if(!nlisprint(login[i]) || !isalnum(login[i]) && login[i] != '_' && login[i] != '-' && login[i] != '.' && login[i] != '[' && login[i] != ']')
//		{
//			return toString("Bad login, character '%c' is not allowed", login[i]);
//		}
//	}

	return "";
}

static void cbClientVerifyLoginPassword(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	//
	// S03: check the validity of the client login/password and send "VLP" message to client
	//

	// reason is empty if everything goes right or contains the reason of the failure
	string reason;
	sint32 uid = -1;
	string login, password, cpassword;
	msgin.serial (login);
	msgin.serial (password);
	cpassword = cryptPassword(password);

	breakable
	{
		CMysqlResult result;
		MYSQL_ROW row;
		sint32 nbrow;
		const CInetAddress &ia = netbase.hostAddress (from);

		reason = sqlQuery("DELETE FROM ban WHERE Date+Duration*60<=NOW();", nbrow, row, result);
		if(!reason.empty()) break;
		string ip = ia.ipAddress();
		reason = sqlQuery("select * from ban where Ip='"+ip+"';", nbrow, row, result);
		if(!reason.empty()) break;
		if(nbrow!=0)
		{
			reason = toString("You(%s) are  banned for %s minutes",row[1],row[3]);
			break;
		}


		reason = checkLogin(login);
		if(!reason.empty()) break;


		reason = sqlQuery("select * from user where Login='"+login+"'", nbrow, row, result);
		if(!reason.empty()) break;

		if(nbrow == 0)
		{
			if(AcceptNewUsers)
			{
				// we accept new user, add it
				reason = sqlQuery("insert into user (Login, Password, Registered) values ('"+login+"', '"+cpassword+"', NOW())", nbrow, row, result);
				if(!reason.empty()) break;
			}
			else
			{
				// can't accept new shard
				reason = toString("Login '%s' doesn't exist and can't be added", login.c_str());
				break;
			}
		}

		// now the user is on the database

		reason = sqlQuery("select * from user where Login='"+login+"'", nbrow, row, result);
		if(!reason.empty()) break;

		uid = atoi(row[0]);

		Output->displayNL("***: %3d Login '%s' Ip '%s'", uid, login.c_str(), ia.asString().c_str());

		string OS, Proc, Mem, Gfx;
		
		msgin.serial (OS);
		msgin.serial (Proc);
		msgin.serial (Mem);
		msgin.serial (Gfx);
		
		if(!OS.empty()) Output->displayNL("OS : %3d %s", uid, OS.c_str());
		if(!Proc.empty()) Output->displayNL("PRC: %3d %s", uid, Proc.c_str());
		if(!Mem.empty()) Output->displayNL("MEM: %3d %s", uid, Mem.c_str());
		if(!Gfx.empty()) Output->displayNL("GFX: %3d %s", uid, Gfx.c_str());

		reason = checkPassword(password, row[2]);
		if(!reason.empty()) break;

		CLoginCookie c;
		c.set((uint32)from, rand(), uid);

		reason = sqlQuery("update user set State='Authorized', Cookie='"+c.setToString()+"' where UId="+toString(uid), nbrow, row, result);
		if(!reason.empty()) break;

		reason = sqlQuery("select * from shard where State='Online'", nbrow, row, result);
		if(!reason.empty()) break;

		// Send success message
		Output->displayNL("---: %3d Ok", uid);
		CMessage msgout ("VLP");

		msgout.serial(reason);

		msgout.serial(nbrow);
		
		// send address and name of all online shards
		while(row != 0)
		{
			// serial the name of the shard
			string shardname = row[3];
			uint8 nbplayers = atoi(row[2]);
			uint32 sid = atoi(row[0]);
			msgout.serial (shardname, nbplayers, sid);
			row = mysql_fetch_row(result);
		}
		netbase.send (msgout, from);
		netbase.authorizeOnly ("CS", from);

		return;
	}

	// Manage error
	Output->displayNL("---: %3d %s", uid, reason.c_str());
	CMessage msgout("VLP");
	msgout.serial(reason);
	netbase.send(msgout, from);
	// FIX: On GNU/Linux, when we disconnect now, sometime the other side doesn't receive the message sent just before.
	//      So it is the other side to disconnect
	//		netbase.disconnect (from);
}

static void cbClientChooseShard(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	//
	// S06: receive "CS" message from client
	//

	string reason;

	breakable
	{
		CMysqlResult result;
		MYSQL_ROW row;
		sint32 nbrow;
		reason = sqlQuery("select * from user where State='Authorized'", nbrow, row, result);
		if(!reason.empty()) break;

		if(nbrow == 0)
		{
			reason = "You are not authorized to select a shard";
			break;
		}

		bool ok = false;
		while(row != 0)
		{
			CLoginCookie lc;
			lc.setFromString(row[5]);
			if(lc.getUserAddr() == (uint32)from)
			{
				ok = true;
				break;
			}
			row = mysql_fetch_row(result);
		}
	
		if(!ok)
		{
			reason = "You are not authorized to select a shard";
			break;
		}

		// it is ok, so we find the wanted shard
		uint32 shardid;
		msgin.serial(shardid);

		CMysqlResult result2;
		MYSQL_ROW row2;
		sint32 nbrow2;
		reason = sqlQuery("select * from shard where ShardId="+toString(shardid), nbrow2, row2, result2);
		if(!reason.empty()) break;

		if(nbrow2 == 0)
		{
			reason = "This shard is not available";
			break;
		}

		uint16 sid = atoi(row2[5]);

		reason = sqlQuery("update user set State='Waiting', ShardId="+toString(shardid)+" where UId="+row[0], nbrow2, row2, result2);
		if(!reason.empty()) break;
		
		CMessage msgout("CS");
		string cookie = row[5];
		msgout.serial(cookie);
		string name = row[1];
		msgout.serial(name);
		sint32 totalScore = atoi(row[6]);
		msgout.serial(totalScore);
		string userTexture = row[8];
		msgout.serial(userTexture);
		string userColorStr = row[9];
		uint32 userColor = 0xffffffff;
		sscanf(userColorStr.c_str(),"0x%x",&userColor);
		msgout.serial(userColor);
		string userTrace = row[10];
		msgout.serial(userTrace);
		string userMesh = row[11];
		msgout.serial(userMesh);

		//TODO MTR We have to change the way LS does this, look at NeLNS standard.
		const std::vector<TServiceId> &sidlist = CUnifiedNetwork::getInstance()->getConnectionList();
		std::vector<TServiceId>::const_iterator itr = sidlist.begin();
		bool fSvc=false;
		while(itr != sidlist.end()) {
			TServiceId sid2 = (*itr);
			if(sid2.get() == sid) {
				CUnifiedNetwork::getInstance()->send(sid2, msgout);
				fSvc=true;
			}
		}
		if(!fSvc)
			nlwarning("Unable to find sid %d in unified network connection list!", sid);

		CMysqlResult result3;
		MYSQL_ROW row3;
		sint32 nbrow3;
		reason = sqlQuery("update texture set LastUsed=NOW(), UsedCount=UsedCount+1 where Name='"+userTexture+"';", nbrow3, row3, result3);

		return;
	}

	// Manage error
	CMessage msgout("SCS");
	msgout.serial(reason);
	netbase.send(msgout, from);
	// FIX: On GNU/Linux, when we disconnect now, sometime the other side doesn't receive the message sent just before.
	//      So it's the other side to disconnect
	//			netbase.disconnect (from);
}

static void cbClientConnection (TSockId from, void *arg)
{
	CCallbackNetBase *cnb = ClientsServer;
	const CInetAddress &ia = cnb->hostAddress (from);
	nldebug("new client connection: %s", ia.asString ().c_str ());
	Output->displayNL ("CCC: Connection from %s", ia.asString ().c_str ());
	cnb->authorizeOnly ("VLP", from);
}

static void cbClientDisconnection (TSockId from, void *arg)
{
	CCallbackNetBase *cnb = ClientsServer;
	const CInetAddress &ia = cnb->hostAddress (from);

	nldebug("new client disconnection: %s", ia.asString ().c_str ());

	string reason;
	
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;
	reason = sqlQuery("select * from user where State!='Offline'", nbrow, row, result);
	if(!reason.empty()) return;
		
	if(nbrow == 0)
	{
		return;
	}
	
	while(row != 0)
	{
		CLoginCookie lc;
		string str = row[5];
		if(!str.empty())
		{
			lc.setFromString(str);
			if(lc.getUserAddr() == (uint32)from)
			{
				// got it, if he is not in waiting state, it s not normal, remove all
				if(row[4] != string("Waiting"))
					sqlQuery("update user set State='Offline', ShardId=-1, Cookie='' where UId="+string(row[0]), nbrow, row, result);
				return;
			}
		}
		row = mysql_fetch_row(result);
	}
}


const TCallbackItem ClientCallbackArray[] =
{
	{ "VLP", cbClientVerifyLoginPassword },
	{ "CS", cbClientChooseShard },
};


void connectionClientInit ()
{
	nlassert(ClientsServer == 0);
	
	ClientsServer = new CCallbackServer();
	nlassert(ClientsServer != 0);

	uint16 port = (uint16) IService::getInstance ()->ConfigFile.getVar ("ClientsPort").asInt();
	ClientsServer->init (port);

	ClientsServer->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));
	ClientsServer->setConnectionCallback(cbClientConnection, 0);
	ClientsServer->setDisconnectionCallback(cbClientDisconnection, 0);
}

void connectionClientUpdate ()
{
	nlassert(ClientsServer != 0);

	try
	{
		ClientsServer->update();
	}
	catch (Exception &e)
	{
		nlwarning ("Error during update: '%s'", e.what ());
	}
}

void connectionClientRelease ()
{
	nlassert(ClientsServer != 0);
	
	delete ClientsServer;
	ClientsServer = 0;
}
