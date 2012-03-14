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

#include <nel/misc/system_info.h>

#include <nel/net/callback_client.h>
#include <nel/net/login_cookie.h>
#include <nel/net/udp_sock.h>

#include "login_client.h"

#include "3d_task.h"
#include "time_task.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;
using namespace NL3D;


//
// Variables
//

CLoginClientMtp::TShardList CLoginClientMtp::ShardList;

string CLoginClientMtp::_GfxInfos;

CCallbackClient *CLoginClientMtp::_CallbackClient = 0;


//
// Functions
//

// Callback for answer of the login password.
bool VerifyLoginPassword;
string VerifyLoginPasswordReason;
void cbVerifyLoginPassword (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	//
	// S04: receive the "VLP" message from LS
	//

	msgin.serial (VerifyLoginPasswordReason);
	if(VerifyLoginPasswordReason.empty())
	{
		uint32 nbshard;
		msgin.serial (nbshard);

		CLoginClientMtp::ShardList.clear ();
		VerifyLoginPasswordReason = "";

		// get the shard list
		for (uint i = 0; i < nbshard; i++)
		{
			CLoginClientMtp::CShardEntry se;
			msgin.serial (se.ShardName, se.ShardNbPlayers, se.ShardId);
			CLoginClientMtp::ShardList.push_back (se);
		}		
	}
	VerifyLoginPassword = true;
}

// Callback for answer of the request shard
bool ShardChooseShard;
string ShardChooseShardReason;
string ShardChooseShardAddr;
string ShardChooseShardCookie;
void cbShardChooseShard (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	//
	// S11: receive "SCS" message from LS
	//

	msgin.serial (ShardChooseShardReason);
	
	if (ShardChooseShardReason.empty())
	{
		msgin.serial (ShardChooseShardCookie);
		msgin.serial (ShardChooseShardAddr);
	}
	ShardChooseShard = true;
}

static TCallbackItem CallbackArray[] =
{
	{ "VLP", cbVerifyLoginPassword },
	{ "SCS", cbShardChooseShard },
};

//

string CLoginClientMtp::authenticate (const string &loginServiceAddr, const string &login, const string &password, uint32 clientVersion)
{
	//
	// S01: connect to the LS
	//
	try
	{
		if(_CallbackClient == 0)
		{
			_CallbackClient = new CCallbackClient();
			_CallbackClient->addCallbackArray (CallbackArray, sizeof(CallbackArray)/sizeof(CallbackArray[0]));
		}
		
		string addr = loginServiceAddr;
		if(addr.find(":") == string::npos)
			addr += ":49997";
		if(_CallbackClient->connected())
			_CallbackClient->disconnect();
		_CallbackClient->connect (CInetAddress(addr));
	}
	catch (ESocket &e)
	{
		delete _CallbackClient;
		_CallbackClient = 0;
		nlwarning ("Connection refused to LS (addr:%s): %s", loginServiceAddr.c_str(), e.what ());
		return "Connection refused to LS";
	}

	//
	// S02: create and send the "VLP" message
	//
	CMessage msgout ("VLP");
	msgout.serial (const_cast<string&>(login));
	msgout.serial (const_cast<string&>(password));
	msgout.serial (clientVersion);

	string OS, Proc, Mem, Gfx;
	OS = CSystemInfo::getOS().c_str();
	Proc = CSystemInfo::getProc().c_str();
	Mem = toString(CSystemInfo::availablePhysicalMemory()) + " " + toString(CSystemInfo::totalPhysicalMemory());
	msgout.serial (OS);
	msgout.serial (Proc);
	msgout.serial (Mem);
	_GfxInfos = C3DTask::getInstance().driver().getDriverInformation();
	_GfxInfos += " / ";
	_GfxInfos += C3DTask::getInstance().driver().getVideocardInformation();
	msgout.serial (_GfxInfos);

	_CallbackClient->send (msgout);

	// wait the answer from the LS
	VerifyLoginPassword = false;
	while (_CallbackClient->connected() && !VerifyLoginPassword)
	{
		_CallbackClient->update ();
		nlSleep(10);
		C3DTask::getInstance().update();
		CTimeTask::getInstance().update();
		if(C3DTask::getInstance().kbPressed(KeyESCAPE))
			return "CLoginClientMtp::authenticate(): user abort";
	}
	
	// have we received the answer?
	if (!VerifyLoginPassword)
	{
		delete _CallbackClient;
		_CallbackClient = 0;
		return "CLoginClientMtp::authenticate(): LS disconnects me";
	}

	if (!VerifyLoginPasswordReason.empty())
	{
		_CallbackClient->disconnect ();
		delete _CallbackClient;
		_CallbackClient = 0;
	}

	return VerifyLoginPasswordReason;
}

string CLoginClientMtp::confirmConnection (uint32 shardListIndex)
{
	nlassert (_CallbackClient != 0 && _CallbackClient->connected());
	nlassert (shardListIndex < ShardList.size());

	//
	// S05: send the client shard choice
	//

	// send CS
	CMessage msgout ("CS");
	msgout.serial (ShardList[shardListIndex].ShardId);
	_CallbackClient->send (msgout);

	// wait the answer
	ShardChooseShard = false;
	while (_CallbackClient->connected() && !ShardChooseShard)
	{
		_CallbackClient->update ();
		nlSleep(10);
		C3DTask::getInstance().update();
		CTimeTask::getInstance().update();
		if(C3DTask::getInstance().kbPressed(KeyESCAPE))
			return "CLoginClientMtp::confirmConnection(): user abort";
	}
	
	// have we received the answer?
	if (!ShardChooseShard)
	{
		delete _CallbackClient;
		_CallbackClient = 0;
		return "CLoginClientMtp::confirmConnection(): LS disconnects me";
	}
	else
	{
		_CallbackClient->disconnect ();
		delete _CallbackClient;
		_CallbackClient = 0;
	}

	if (!ShardChooseShardReason.empty())
	{
		return ShardChooseShardReason;
	}

	// ok, we can try to connect to the good front end

	nlinfo("addr: '%s' cookie: %s", ShardChooseShardAddr.c_str(), ShardChooseShardCookie.c_str());

	return "";
}

string CLoginClientMtp::connectToShard (uint32 shardListIndex, CInetAddress &ip, string &cookie)
{
	string res = confirmConnection (shardListIndex);
	if (!res.empty()) return res;

	ip = ShardChooseShardAddr;
	cookie = ShardChooseShardCookie;
	
	return "";
}


uint32 CLoginClientMtp::shardIdToIndex (uint32 shardId)
{
	TShardList::iterator it;
	int index = 0;
	for(it=ShardList.begin();it!=ShardList.end();it++)
	{
		if((*it).ShardId == shardId)
			return index;
		index++;
	}
	return 0;
}