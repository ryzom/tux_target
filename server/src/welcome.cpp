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

#include "stdpch.h"

#include "nel/misc/types_nl.h"

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <list>

#include <nel/misc/debug.h>
#include <nel/misc/config_file.h>
#include <nel/misc/displayer.h>
#include <nel/misc/command.h>
#include <nel/misc/log.h>

#include <nel/net/service.h>
#include <nel/net/unified_network.h>
#include <nel/net/login_cookie.h>

#include "welcome.h"
#include "entity_manager.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//

/// association between cookie in string and username

class CUserInformation
{
public:

	CUserInformation(const string &name,sint32 totalScore,const string &texture, uint32 color,const string &trace,const string &mesh)
	{
		this->name		 = name;
		this->totalScore = totalScore;
		this->texture	 = texture;
		this->color.setPacked(color);
		this->trace		 = trace;
		this->mesh		 = mesh;
	}

	string name;
	sint32 totalScore;
	string texture;
	CRGBA  color;
	string trace;
	string mesh;
protected:
private:
};

//static map<string, pair<string, pair<string, sint32> > > UserIdNameAssociations;
static map<string, CUserInformation > UserIdNameAssociations;

static string ListenAddr;

string getUserFromCookie(const string &cookie, sint32 &totalScore,string &userTexture, CRGBA &color ,string &userTrace, string &userMesh)
{
	//map<string, pair<string, pair<string, sint32> > >::iterator it = UserIdNameAssociations.find(cookie);
	map<string, CUserInformation >::iterator it = UserIdNameAssociations.find(cookie);
	if(it == UserIdNameAssociations.end())
	{
		totalScore = 0;
		return "";
	}
	else
	{
		totalScore	= (*it).second.totalScore;
		userTexture = (*it).second.texture;
		color		= (*it).second.color;
		userTrace	= (*it).second.trace;
		userMesh	= (*it).second.mesh;
		return (*it).second.name;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// CONNECTION TO THE LOGIN SERVICE ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cbLSChooseShard (CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	// the LS warns me that a new client want to come in my shard

	//
	// S07: receive the "CS" message from LS and send the "CS" message to the selected FES
	//

	string cookie, userName;
	sint32 totalScore;
	string userTexture = "";
	string userTrace = "";
	uint32 userColor = 0xffffffff;
	string userMesh = "";

	msgin.serial(cookie, userName, totalScore);
	//TODO remove this test when version will be = to 6
	if(msgin.getPos() < (sint32)msgin.length())
	{
		msgin.serial(userTexture);
		//nlinfo("LS sent texture : %s",userTexture.c_str());
	}
	if(msgin.getPos() < (sint32)msgin.length())
	{
		msgin.serial(userColor);
		nlinfo("LS sent user color : 0x%x",userColor);
	}
	if(msgin.getPos() < (sint32)msgin.length())
	{
		msgin.serial(userTrace);
		nlinfo("LS sent user trace: %s",userTrace.c_str());
	}
	if(msgin.getPos() < (sint32)msgin.length())
	{
		msgin.serial(userMesh);
		nlinfo("LS sent user mesh : %s",userMesh.c_str());
	}
	
	// always accept clients
	string reason = "";

	//
	// S09: receive "SCS" message from FES and send the "SCS" message to the LS
	//

	CMessage msgout("SCS");

	msgout.serial(reason);
	msgout.serial(cookie);

	if(reason.empty())
		msgout.serial(ListenAddr);

	CUnifiedNetwork::getInstance()->send("LS", msgout);

	//UserIdNameAssociations.insert(make_pair(cookie, make_pair(userName,make_pair(userTexture,totalScore))));
	UserIdNameAssociations.insert(make_pair(cookie, CUserInformation(userName,totalScore,userTexture,userColor,userTrace,userMesh)));

	nlinfo("Client %s will come with cookie %s to ip '%s' with total score %d userTexture '%s'", userName.c_str(), cookie.c_str(), ListenAddr.c_str(), totalScore,userTexture.c_str());
}

void cbFailed(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	// I can't connect to the Login Service, just nlerror();
	string reason;
	msgin.serial(reason);
	nlerror(reason.c_str());
}

void cbLSDisconnectClient(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	// the LS tells me that i have to disconnect a client
/*
	uint32 userid;
	msgin.serial(userid);

	map<uint32, TServiceId>::iterator it = UserIdSockAssociations.find(userid);
	if(it == UserIdSockAssociations.end())
	{
		nlwarning("Can't disconnect the user %d, he is not found", userid);
	}
	else
	{
		CMessage msgout("DC");
		msgout.serial(userid);
		CUnifiedNetwork::getInstance()->send((*it).second, msgout);
	}*/
}

// connection to the LS, send the identification message
void cbLSConnection (const std::string &serviceName, TServiceId  sid, void *arg)
{
	CMessage msgout("WS_IDENT");
	sint32 shardId;
	
	try
	{
		shardId = IService::getInstance()->ConfigFile.getVar("ShardId").asInt();
	}
	catch(Exception &)
	{
		shardId = -1;
	}

	if(shardId == -1)
	{
		nlerror("ShardId variable in the config file must be valid(>0)");
	}

	msgout.serial(shardId);

	// send online players

	std::list <CEntity*> &ent = CEntityManager::getInstance().entities();
	sint32 nbplayers = 0;
	for(CEntityManager::EntityConstIt it = ent.begin(); it != ent.end(); it++)
	{
		if((*it)->type() == CEntity::Client)
		{
			nbplayers++;
		}
	}
	msgout.serial(nbplayers);
	for(CEntityManager::EntityConstIt it = ent.begin(); it != ent.end(); it++)
	{
		if((*it)->type() == CEntity::Client)
		{
			CClient *cl = dynamic_cast<CClient*>(*it);
			sint32 uid = cl->uid();
			msgout.serial(uid);
		}
	}

	CUnifiedNetwork::getInstance()->send(sid, msgout);

	nlinfo("Connected to %s-%hu and sent identification with shardId '%d'", serviceName.c_str(), sid.get(), shardId);
}





// Callback Array for message from LS
TUnifiedCallbackItem LSCallbackArray[] =
{
	{ "CS", cbLSChooseShard },
	{ "DC", cbLSDisconnectClient },
	{ "FAILED", cbFailed },
};

static uint ThreadId = 0;
static CSynchronized<vector<pair<uint32, uint8> > > ConnectedClients("ConnectedClients");

/// Init the service, load the universal time.
void initWelcome()
{
	ThreadId = getThreadId();

	if(IService::getInstance()->ConfigFile.getVar("ShardId").asInt() == 0)
	{
		nlinfo("Running server in LAN mode");
		return;
	}

	nlinfo("Running server in Internet mode");

	for(uint i = 0; i < IService::getInstance()->ConfigFile.getVar("LSHost").size(); i++)
	{
		// add a connection to the LS
		string LSAddr = IService::getInstance()->ConfigFile.getVar("LSHost").asString(i);

		// add default port if not set by the config file
		if(LSAddr.find(":") == string::npos)
			LSAddr += ":49999";

		CUnifiedNetwork::getInstance()->addCallbackArray(LSCallbackArray, sizeof(LSCallbackArray)/sizeof(LSCallbackArray[0]));
		CUnifiedNetwork::getInstance()->setServiceUpCallback("LS", cbLSConnection, 0);
		CUnifiedNetwork::getInstance()->addService("LS", LSAddr);
	}

	if(IService::getInstance()->ConfigFile.exists("ListenAddr"))
		ListenAddr = IService::getInstance()->ConfigFile.getVar("ListenAddr").asString();
	else
		ListenAddr = CInetAddress::localHost().ipAddress();

	if(ListenAddr.find(":") == string::npos)
	{
		ListenAddr += ":" + IService::getInstance()->ConfigFile.getVar("TcpPort").asString();
	}

	nlinfo("Listen address send to client will be '%s'", ListenAddr.c_str());
}

void clientConnected(const string &cookie, bool connected)
{
	if(ThreadId == getThreadId())
	{
		CLoginCookie c;
		c.setFromString(cookie);
		
		if(!c.isValid())
		{
			nlwarning("Invalid cookie '%s'", cookie.c_str());
			return;
		}

		CMessage msgout("CC");
		
		uint32 userid = c.getUserId();
		msgout.serial(userid);
		
		uint8 con = connected?1:0;
		msgout.serial(con);
		
		nlinfo("clientConnected(%s, %d) uid %u direct send", cookie.c_str(), connected, userid);

		CUnifiedNetwork::getInstance()->send("LS", msgout);
	}
	else
	{
		CLoginCookie c;
		c.setFromString(cookie);
		
		if(!c.isValid())
		{
			nlwarning("Invalid cookie '%s'", cookie.c_str());
			return;
		}
		
		uint32 userid = c.getUserId();
		uint8 con = connected?1:0;
		
		nlinfo("clientConnected(%s, %d) uid %u delayed send", cookie.c_str(), connected, userid);

		CSynchronized<vector<pair<uint32, uint8> > >::CAccessor access(&ConnectedClients);
		access.value().push_back(make_pair(userid,con));
	}
}

void updateConnectedClients()
{
	H_AUTO(updateConnectedClients);
	CSynchronized<vector<pair<uint32, uint8> > >::CAccessor access(&ConnectedClients);

	for(uint i = 0; i < access.value().size(); i++)
	{
		CMessage msgout("CC");
		
		uint32 userid = access.value()[i].first;
		msgout.serial(userid);
		
		uint8 con = access.value()[i].second;
		msgout.serial(con);
		
		nlinfo("updateConnectedClient %d uid %u send delayed client", con, userid);
		
		CUnifiedNetwork::getInstance()->send("LS", msgout);
	}

	access.value().clear();
}
