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

#include <time.h>
#include <zlib.h>
#include <errno.h>

#include <nel/misc/path.h>
#include <nel/misc/sha1.h>

#include <nel/net/login_cookie.h>

#include "command.h"
#include "network.h"
#include "welcome.h"
#include "level_manager.h"
#include "net_callbacks.h"
#include "entity_manager.h"
#include "session_manager.h"


//
// Namespaces
//

using namespace std;
using namespace NLNET;
using namespace NLMISC;


//
// Functions
//

static void cbChat(CClient *c, CNetMessage &msgin)
{
	string msg;
	msgin.serial(msg);

	// remove % to avoid printf error
	for(uint i = 0; i < msg.size(); i++) if(msg[i] == '%') msg[i] = ' ';

	if(c->canSpeak())
		CNetwork::getInstance().sendChat("<"+c->name()+"> "+msg);
	else
		CNetwork::getInstance().sendChat(c->id(),"<"+c->name()+"> "+msg,false);
}

static void cbCommand(CClient *c, CNetMessage &msgin)
{
	nlinfo("cbCommand");
	string icmd;
	msgin.serial(icmd);

	if(!c->canSpeak())
		return;
	
	//string cmd = toLower(icmd);
	string cmd = icmd;//toLower(icmd);

	if(cmd.substr(0,4)=="help")
	{
		if(c->isAdmin() || c->isModerator())
		{
			CNetwork::getInstance().sendChat(c->id(),string("/reset (CTRL+F6): hard reset of session (could crash)"));
			CNetwork::getInstance().sendChat(c->id(),string("/reparsePath"));
			CNetwork::getInstance().sendChat(c->id(),string("/playerList"));
			CNetwork::getInstance().sendChat(c->id(),string("/report nick info"));
			CNetwork::getInstance().sendChat(c->id(),string("/ban nick duration"));
			CNetwork::getInstance().sendChat(c->id(),string("/kick nick"));
			CNetwork::getInstance().sendChat(c->id(),string("/forceEnd (CTRL+F5): force end of session (safe)"));
			CNetwork::getInstance().sendChat(c->id(),string("/forceMap mapname : try to force this map for next level"));
		}
		else
		{
			CNetwork::getInstance().sendChat(c->id(),string("/voteMap mapname : vote to start this map for next level"));
		}
		return;
	}


	if(cmd.substr(0,8)=="votemap ")
	{
		string arg = cmd.substr(8);
		if(!arg.empty())
			c->voteMap(arg);
		CNetwork::getInstance().sendChat(c->name()+" executed: /"+cmd);
	}
	else if(cmd.substr(0,2)=="v ")
	{
		string arg = cmd.substr(2);
		if(!arg.empty())
			c->voteMap(arg);
		CNetwork::getInstance().sendChat(c->name()+" executed: /"+cmd);
	}
	else if(cmd.substr(0,8)=="petition")
	{
		string msg = "shout " + c->name() + " need assistance";
		CNetwork::getInstance().forwardToPublicChat(msg);
		CNetwork::getInstance().sendChat(c->name()+" executed: /"+cmd);
	}
	else if(c->isAdmin() || c->isModerator())
	{
		//CNetwork::getInstance().networkTask().addCommand(cmd);
		CNetwork::getInstance().sendChat(c->name()+" executed: /"+cmd);
		CCommand::execute(c, cmd, *InfoLog);
	}
	else
	{
		CNetwork::getInstance().sendChat(c->name()+" tried to execute the admin command /"+cmd);
	}
}

static void cbForce(CClient *c, CNetMessage &msgin)
{
	// new client update
	CVector force;
	msgin.serial(force);
	c->setForce(force);
}

static void cbLogin(CClient *c, CNetMessage &msgin)
{
	string cookie;
	string login, password;
	CRGBA color;
	string texture;
	string trace;
	string mesh;
	uint32 networkVersion = CNetwork::getInstance().version();
	string error;

	c->Score = 0;
	
	nlinfo("New client login");
	
	// first, check the version used by client
	msgin.serial(networkVersion); 

	if(networkVersion < CNetwork::getInstance().version())
	{
		nlinfo("bad client version %d should be %d",networkVersion,CNetwork::getInstance().version());
		string reason = toString("login failed: bad client version(%d)! Get latest one on Mtp Target web site", networkVersion);
		CNetMessage msgout(CNetMessage::Error);
		msgout.serial(reason);
		CNetwork::getInstance().send(c->id(), msgout);
		return;
	}
	c->networkVersion = networkVersion;

	nlinfo("Version ok");
	// now we know the version is compatible, get them

	// check the password
	msgin.serial(cookie, login, password, color, texture);

	login = strlwr(login);

	nlinfo("cookie:%s login:%s texture:%s color:%d %d %d %d",cookie.c_str(),login.c_str(),texture.c_str(),color.R,color.G,color.B,color.A);
	
	if(error.empty())
	{
		if(cookie.empty())
		{
			// check in normal way
			error = CEntityManager::getInstance().check(login, password, false, c->Score);
		}
		else
		{
			// already checks in LS
			nlinfo("Receive a client with cookie %s", cookie.c_str());
			login = getUserFromCookie(cookie, c->Score, texture, color, trace, mesh);
			nlinfo("score = %d , texture = %s", c->Score, texture.c_str());
			if(login.empty())
			{
				error = "Bad cookie identification";
			}
			else
			{
				error = CEntityManager::getInstance().check(login, password, true, c->Score);
			}
		}
	}

	if(error.empty())
	{
		// init() must be after the check() call
		c->init(login,texture,color,trace,mesh);
		c->Cookie = cookie;
		if(!cookie.empty())
		{
			CLoginCookie lc;
			lc.setFromString(cookie);
			c->uid(lc.getUserId());
		}
	
		CEntityManager::getInstance().login(c);
	}
	else
	{
		nlwarning("Login client '%s' failed: %s", login.c_str(), error.c_str());
		CNetMessage msgout(CNetMessage::Error);
		msgout.serial(error);
		CNetwork::getInstance().send(c->id(), msgout);
	}

	{
		char d[80];
		time_t ltime;
		time(&ltime);
		struct tm *today = localtime(&ltime);
		strftime(d, 80, "%Y %m %d %H %M %S", today);
		FILE *fp = fopen("connection.stat", "ab");
		if(fp)
		{
			fprintf(fp, "%u %s", ltime, d);
			fprintf(fp, " %c", (error.empty()?'+':'?'));
			fprintf(fp, " '%s' '%s'", login.c_str(), texture.c_str());
			fprintf(fp, " '%p'", c->sock());
			if(!error.empty()) fprintf(fp, " '%s'", error.c_str());
			fprintf(fp, "\n");
			fclose(fp);
		}
		fp = fopen("clients_count.stat", "ab");
		if(fp)
		{
			fprintf(fp, "%u %s c %d\n", ltime, d, CEntityManager::getInstance().humanClientCount());
			fclose(fp);
		}
	}
}

static void cbOpenClose(CClient *c, CNetMessage &msgin)
{
	nlinfo("cbOpenClose");
	TTime currentTime = CTime::getLocalTime();
	
	if(CSessionManager::getInstance().endTime() != 0 || CSessionManager::getInstance().startTime() == 0 || currentTime < CSessionManager::getInstance().startTime())
	{
		// ignoring client update if not in session
		nlinfo("Can't open or close client '%s' because there's no session", c->name().c_str());
		return;
	}
	
	if (c->openClose())
	{
		CNetMessage msgout(CNetMessage::OpenClose);
		uint8 eid = c->id();
		msgout.serial(eid);
		CNetwork::getInstance().send(msgout);
	}
}

static void cbEditMode(CClient *c, CNetMessage &msgin)
{
	nlinfo("cbEditMode");
	uint8 editMode;
	msgin.serial(editMode);
	if(c->isAdmin())
		CSessionManager::getInstance().editMode(editMode);
}

static void cbUpdateElement(CClient *c, CNetMessage &msgin)
{
	nlinfo("cbUpdateElement");
	uint8 elementType;
	msgin.serial(elementType);
	uint8 elementId;
	msgin.serial(elementId);
	uint8 selectedBy;
	msgin.serial(selectedBy);
	CVector pos;
	msgin.serial(pos);
	CVector eulerRot;
	msgin.serial(eulerRot);

	if(c->isAdmin() && CSessionManager::getInstance().currentStateName()=="Running")
	{
		CNetMessage msgout(CNetMessage::UpdateElement);
		msgout.serial(elementType);
		msgout.serial(elementId);
		msgout.serial(selectedBy);
		msgout.serial(pos);
		msgout.serial(eulerRot);
		CNetwork::getInstance().sendAllExcept(c->id(),msgout);

		if(CSessionManager::getInstance().editMode()==0)
			CSessionManager::getInstance().editMode(1);
		if(elementType==CEditableElementCommon::Module)
			CLevelManager::getInstance().currentLevel().updateModule(elementId,pos,eulerRot);
		else if(elementType==CEditableElementCommon::StartPosition)
			CLevelManager::getInstance().currentLevel().updateStartPoint(elementId,pos,eulerRot);
	}
	
}

static void cbReady(CClient *c, CNetMessage &msgin)
{
	c->Ready = true;
	c->WaitingReady = false;
	nlinfo("Client '%s' is ready to play", c->name().c_str());
	CNetMessage msgout(CNetMessage::Ready);
	uint8 eid = c->id();
	msgout.serial(eid);
	CNetwork::getInstance().send(msgout);
}

static void cbRequestCRCKey(CClient *c, CNetMessage &msgin)
{
	c->WaitingReadyTimeoutStart = CTime::getLocalTime();
	CNetMessage msgout(CNetMessage::RequestCRCKey);

	string fn;
	msgin.serial(fn);
	CHashKey hashKey;
	msgin.serial(hashKey);

	nlinfo("cbRequestCRCKey from '%s' for the file '%s'", c->name().c_str(), fn.c_str());

	string fns = CFile::getFilename(fn);
	string path = CPath::lookup(fns, false, false);

	CHashKey serverHashKey;
	string serverfn;
	if(path.empty())
		serverfn = "";
	else
	{
		serverHashKey = getSHA1(path);
		if(serverHashKey==hashKey)
		{
			serverfn = "";
		}
		else
		{
			nlinfo("cbRequestCRCKey : key difeerent");
			serverfn = CFile::getFilename(path);
		}
	}

	msgout.serial(serverfn);
	msgout.serial(serverHashKey);
	CNetwork::getInstance().send(c->id(),msgout);	
}
	
static void cbRequestDownload(CClient *c, CNetMessage &msgin)
{
	c->WaitingReadyTimeoutStart = CTime::getLocalTime();
	CNetMessage msgout(CNetMessage::RequestDownload);

	string fn;
	msgin.serial(fn);

	nlinfo("cbRequestDownload from '%s' for the file '%s'", c->name().c_str(), fn.c_str());
	
	string fns = CFile::getFilename(fn);
	string path = CPath::lookup(fns, false, false);
	string packedPath = CPath::lookup(CFile::getFilename(path)+".gz", false, false);
	if(path.empty())
	{
		nlwarning("Client '%s' wants an unknown file '%s'", c->name().c_str(), fn.c_str());
		string res = toString("ERROR:File '%s' is not found", fn.c_str());
		msgout.serial(res);
		CNetwork::getInstance().send(c->id(),msgout);
		return;
	}

	vector<uint8> buf;
	buf.resize(8000);
	uint8 *ptr = &(*(buf.begin()));
	
	bool packedFileUpToDate = CFile::getFileModificationDate(packedPath) >= CFile::getFileModificationDate(path);


	if(!packedFileUpToDate)
	{
		CHashKey serverHashKey = getSHA1(path);
		/*
		CEntityManager::CEntities::CReadAccessor acces(CEntityManager::getInstance().entities());
		for(CEntityManager::EntityConstIt it = acces.value().begin(); it != acces.value().end(); it++)
		{
			nlassert(*it);
			if((*it)->type() != CEntity::Client || (*it)==c)
				continue;
			
			CClient *e = (CClient *)(*it);
			uint8 nid = e->id();
			CNetMessage msgout(CNetMessage::RequestCRCKey);
			msgout.serial(fn);
			msgout.serial(serverHashKey);
			CNetwork::getInstance().send(nid, msgout);
		}
		*/
		CNetMessage msgout(CNetMessage::RequestCRCKey);
		msgout.serial(fn);
		msgout.serial(serverHashKey);
		CNetwork::getInstance().sendAllExcept(c->id(), msgout);
	}

	if(packedPath.empty() || !packedFileUpToDate )
	{
		if(packedPath.empty())
			packedPath = path + ".gz";
		FILE *fp = fopen(path.c_str(), "rb");
		gzFile gzfp = gzopen(packedPath.c_str(), "wb");
		while (!feof(fp)) 
		{
			uint32 res = fread(ptr, 1, 8000, fp);
			gzwrite(gzfp,ptr,res);
		}
		fclose(fp);
		gzclose(gzfp);
		
	}

	uint32 part = 0;
	msgin.serial(part);
	nlinfo("Client '%s' wants file '%s' part %d", c->name().c_str(), fns.c_str(), part);

	FILE *fp = fopen(packedPath.c_str(), "rb");
	if(!fp)
	{
		nlwarning("Can't open file '%s'", packedPath.c_str());
		string res = toString("ERROR:Can't open file '%s'", packedPath.c_str());
		msgout.serial(res);
		CNetwork::getInstance().send(c->id(),msgout);
		return;
	}

	if (fseek(fp, part, SEEK_SET) != 0)
	{
		nlwarning("Can't seek file '%s' to part %d", packedPath.c_str(), part);
		string res = toString("ERROR:Can't seek file '%s' part %d", packedPath.c_str(), part);
		msgout.serial(res);
		CNetwork::getInstance().send(c->id(),msgout);
		fclose(fp);
		return;
	}

	uint32 fileSize = CFile::getFileSize(fp);

	uint32 res = fread(ptr, 1, 8000, fp);
	bool eof = false;
	if(res != 8000)
	{
		if(feof(fp))
		{
			// we are at the end of the file, resize to the good size
			buf.resize(res);
			eof = true;
		}
		else
		{
			// problem during reading
			nlwarning("Failed to read file '%s' to part %d: %s", packedPath.c_str(), part, strerror(errno));
			string res = toString("ERROR:Failed to read file '%s' part %d: %s", packedPath.c_str(), part, strerror(errno));
			msgout.serial(res);
			CNetwork::getInstance().send(c->id(),msgout);
			fclose(fp);
			return;
		}
	}

	// empty string means no error
	string str = toString("FILE:%s",CFile::getFilename(path).c_str());
	msgout.serial(str);
	msgout.serial(fileSize);
	msgout.serialCont(buf);

	nlinfo("the file eof is : %d size = %d", eof, res);
	msgout.serial(eof);

	CNetwork::getInstance().send(c->id(),msgout);
	fclose(fp);
}

static void cbUpdate(CClient *c, CNetMessage &msgin)
{
	TTime CurrentTime = CTime::getLocalTime();

	uint8 pingnb;
	msgin.serial(pingnb);

	if(c->LastSentPing.empty())
	{
		nlwarning ("Received an ack of an update without info sending from %hu '%s'", (uint16)c->id(), c->name().c_str());
		return;
	}

next:
	if(c->LastSentPing.empty())
	{
		nlwarning ("empty ping array from %hu '%s'", (uint16)c->id(), c->name().c_str());
		return;
	}
	pair<uint8, TTime> p = c->LastSentPing.front();
	if(pingnb < p.first)
	{
		nlwarning ("Received an ack of an update with bad sync ping should be %u and is %u from %hu '%s'", p.first, pingnb, (uint16)c->id(), c->name().c_str());
		return;
	}
	else if(pingnb > p.first)
	{
		nlwarning ("Received an ack of an update with bad sync ping should be %u and is %u from %hu '%s'", p.first, pingnb, (uint16)c->id(), c->name().c_str());
		c->LastSentPing.pop();
		goto next;
	}

	if(CurrentTime < p.second)
	{
		nlwarning ("Received an ack of an update with bad sync from %hu '%s'", (uint16)c->id(), c->name().c_str());
		return;
	}

	if(SavePingStat)
		c->StatPing.push_back(make_pair(CurrentTime,CurrentTime - p.second));

	c->Ping.addValue((uint16)(CurrentTime - p.second));
//	nlinfo("*********** receive the pong %u %"NL_I64"u", pingnb, CurrentTime);
//	nlinfo("%"NL_I64"u Ping for '%s' is %d (%"NL_I64"d)", CurrentTime, c->name().c_str(), c->Ping.getSmoothValue(), CurrentTime - p.second);
	c->LastSentPing.pop();
}




//
// Callback handler
//

#define SWITCH_CASE(_n) case CNetMessage::_n: { H_AUTO(_n); cb##_n(c, msgin); } break

void netCallbacksHandler(CClient *c, CNetMessage &msgin)
{
//	nldebug("NET: Received message type %hu size %u from %hu '%s'", (uint16)msgin.type(), msgin.length(), c->id(), c->name().c_str());

	nlassert(c);
	nlassert(c->type() == CEntity::Client);
	if(CEntityManager::getInstance().getById(c->id())==0)
	{
		nlwarning("netCallbacksHandler(%d) cannot do cb since client does not exist",msgin.type());
		return;
	}
	
	switch(msgin.type())
	{
	SWITCH_CASE(Chat);
	SWITCH_CASE(Command);
	SWITCH_CASE(EditMode);
	SWITCH_CASE(Force);
	SWITCH_CASE(Login);
	SWITCH_CASE(OpenClose);
	SWITCH_CASE(Ready);
	SWITCH_CASE(RequestDownload);
	SWITCH_CASE(RequestCRCKey);
	SWITCH_CASE(Update);
	SWITCH_CASE(UpdateElement);
	
	default: nlinfo("Received an unknown message type %hu", (uint16)msgin.type()); break;
	}

}


//
// Commands
//
