// This file is part of Mtp Target.
// Copyright (C) 2008 Vialek
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// 
// Vianney Lecroart - gpl@vialek.com


//
// Includes
//

#include "stdpch.h"

#include <nel/misc/types_nl.h>

#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <windows.h>
#endif

#include <nel/misc/path.h>

#include "gui.h"
#include "level.h"
#include "graph.h"
#include "module.h"
#include "3d_task.h"
#include "hud_task.h"
#include "chat_task.h"
#include "intro_task.h"
#include "mtp_target.h"
#include "login_client.h"
#include "network_task.h"
#include "net_callbacks.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "config_file_task.h"
#include "external_camera_task.h"
#include "../../common/custom_floating_point.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//

static void cbChat(CNetMessage &msgin)
{
//	nlinfo("NET: cbChat");
	ucstring msg;
	bool systemMessage;
	msgin.serial(systemMessage);
	while(msgin.getPos() < (sint32)msgin.length())
	{
		msgin.serial(msg);
		if(systemMessage)
		{
			CHudTask::instance().addSysMessage(convertVariableString(msg));
		}
		else
		{
			vector<ucstring> lines;
			explode(convertVariableString(msg), ucstring("\n"), lines, true);
			for(uint i = 0; i < lines.size(); i++)
			{
				CChatTask::instance().addLine(lines[i]);
				SessionString += toString("%hu CH %s\n", 0, lines[i].toUtf8().c_str());
			}
		}
	}
}

static void cbError(CNetMessage &msgin)
{
	string reason;
	msgin.serial(reason);
	nlinfo("NET: cbError reason='%s'", reason.c_str());

#ifdef NL_OS_WINDOWS
	if(reason.find("bad client version") != ucstring::npos)
	{
		// the client is not up to date, ask the user to update
		if(MessageBoxA(NULL, "Your client is not up to date. You *must* download a new version from the mtp target web site.\r\nDo you want to automatically quit and go to the download page?", "Warning", MB_ICONWARNING|MB_YESNO) == IDYES)
		{
			openURL("http://mtptarget.free.fr/download.php");
			exit(0);
		}
	}
#endif

	CMtpTarget::instance().error(reason);
}

static void cbLogin(CNetMessage &msgin)
{
	bool self;
	uint8 eid;
	ucstring name;
	sint32 totalScore;
	CRGBA color;
	string texture;
	bool spec;
	bool oc = false;
	string trace = "trace";
	string mesh = "pingoo";

	msgin.serial(self, eid, name, totalScore);
	msgin.serial(color, texture, spec);

	string levelName;
	float timeBeforeTimeout;
	if(self)
	{
		msgin.serial(levelName, timeBeforeTimeout);
		// bugfix: when loading a team map, the server send the full path instead of the level file name
		levelName = CFile::getFilename(levelName);
	}

	msgin.serial(oc);
	msgin.serial(trace);
	msgin.serial(mesh);
	bool fullversion;
	msgin.serial(fullversion);

	if(self)
	{
		FullVersion = fullversion;
		//FastExit = false;
	}

//	nlinfo("NET: cbLogin self=%d eid=%hu name='%s' totalScore='%d' color=(%d,%d,%d) texture='%s' spec=%d oc=%d trace='%s' mesh='%s'", 
//		self, (uint16)eid, name.toUtf8().c_str(), totalScore, color.R, color.G, color.B, texture.c_str(), spec, oc, trace.c_str(), mesh.c_str());
		
//	nldebug("player list.size = %d", CEntityManager::instance().size());

	CEntityManager::instance().add(eid, name, totalScore, color, texture, spec, self, trace, mesh, fullversion);

	if(oc)
		CEntityManager::instance()[eid].swapOpenClose(false);

	if(self)
	{
		CMtpTarget::instance().displayTutorialInfo(totalScore<=CConfigFileTask::instance().configFile().getVar("MinTotalScoreToHideTutorial").asInt());

		if(CMtpTarget::instance().displayTutorialInfo())
			CGuiObjectManager::instance().displayFrame("help1");

//		nlinfo("levelName='%s' timeBeforeTimeout=%f", levelName.c_str(), timeBeforeTimeout);
		
		if (!levelName.empty())
			CMtpTarget::instance().startSession(0, timeBeforeTimeout/1000.0f, levelName, false, ucstring(""), ucstring(""), true);
		else
			CMtpTarget::instance().timeBeforeTimeout(timeBeforeTimeout/1000.0f);
	}

	CGuiCustom::instance().onLogin(name);
}

static void cbLogout(CNetMessage &msgin)
{
	uint8 eid;
	msgin.serial(eid);
//	nlinfo("NET: cbLogout eid=%hu", (uint16)eid);

	// check if the player exists
	if(!CEntityManager::instance().exist(eid)) { nlwarning("The eid doesn't exist"); return; }

	// if it's my eid, it means that i have to disconnect because i was kicked out from the server
	if(CMtpTarget::instance().controler().getControledEntity()==eid)
	{
		CMtpTarget::instance().error("ErrKicked");
		//nlerror("You have been kicked");
	}

	CGuiCustom::instance().onLogout(CEntityManager::instance()[eid].name());
	CEntityManager::instance().remove(eid);
}

static void cbOpenClose(CNetMessage &msgin)
{
	uint8 eid;
	msgin.serial(eid);
//	nlinfo("NET: cbOpenClose eid=%hu (%s)", (uint16)eid,CEntityManager::instance()[eid].name().toUtf8().c_str());

	// check if the player exists
	if(!CEntityManager::instance().exist(eid)) { nlwarning("The eid doesn't exist"); return; }
	
	//CEntityManager::instance()[eid].addOpenCloseKey = true;
	CEntityManager::instance()[eid].swapOpenClose();

	SessionString += toString("%hu OC\n", (uint16)eid);
}

/*static void cbUpdate(CNetMessage &msgin)
{
	//nlinfo("cbUpdate %f",CTimeTask::instance().time());
	float rsxTime;
	uint8 eid;
	CVector pos;
	CVector dpos;
	CVector ddpos;
	//uint16 ping;

	// reply to the update first (used for the ping)
	uint8 pingnb;
	msgin.serial(pingnb);
	CNetMessage msgout(CNetMessage::Update);
	msgout.serial(pingnb);
//	nlinfo("*********** send the pong %u %"NL_I64"u", pingnb, CTime::getLocalTime());
	CNetworkTask::instance().send(msgout);
	
	//msgin.serial (rsxTime);

	//while(msgin.getPos() < (sint32)msgin.length())
	//nlassert(CEntityManager::instance().updateListId.size());
	list <uint8 >::iterator it;
	//for(it=CEntityManager::instance().updateListId.begin();it!=CEntityManager::instance().updateListId.end();it++)
	for(eid=0;eid<255;eid++)
	{
		if(CEntityManager::instance().exist(eid))
		{
			if(!CEntityManager::instance()[eid].isLocal())
				CEntityManager::instance()[eid].interpolator().dt(MT_NETWORK_UPDATE_PERIODE);

			if(msgin.getPos() >= (sint32)msgin.length())
			{
				nlwarning("cbUpdate not enough data in message (stopped at eid = %d)",eid);
				break;
			}
			//eid = *it;
			float deltaCoef = 100;

// 			uint8 dxx,sxx;
// 			msgin.serial(sxx,dxx);
// 			dpos.x = convert8_8fp(sxx,dxx);
// 			uint8 dxy,sxy;
// 			msgin.serial(sxy,dxy);
// 			dpos.y = convert8_8fp(sxy,dxy);
// 			uint8 dxz,sxz;
// 			msgin.serial(sxz,dxz);
// 			dpos.z = convert8_8fp(sxz,dxz);

			uint32 bits;
			msgin.serial(bits);
			packBit32 pb32(bits);
			uint32 ndx,nsx;
			pb32.unpackBits(nsx,6);
			pb32.unpackBits(ndx,4);//z
			//		nlinfo(">>>>z dx/ndx = %d/%d ",dxz,ndx);
			//		nlinfo(">>>>z sx/nsx = %d/%d ",sxz,nsx);
			//nlassert(dxz==ndx);
			//nlassert(sxz==nsx);
			dpos.z = convert8_8fp(nsx,ndx);
			pb32.unpackBits(nsx,6);
			pb32.unpackBits(ndx,4);//y
			//		nlinfo(">>>>y dx/ndx = %d/%d ",dxy,ndx);
			//		nlinfo(">>>>y sx/nsx = %d/%d ",sxy,nsx);
			//nlassert(dxy==ndx);
			//nlassert(sxy==nsx);
			dpos.y = convert8_8fp(nsx,ndx);
			pb32.unpackBits(nsx,6);
			pb32.unpackBits(ndx,4);//x
			//		nlinfo(">>>>x dx/ndx = %d/%d ",dxx,ndx);
			//		nlinfo(">>>>x sx/nsx = %d/%d ",sxx,nsx);
			//nlassert(dxx==ndx);
			//nlassert(sxx==nsx);
			dpos.x = convert8_8fp(nsx,ndx);
			

			if(CEntityManager::instance().exist(eid))
			{
				pos = CEntityManager::instance()[eid].LastSent2OthersPos + dpos;
				CEntityManager::instance()[eid].LastSent2OthersPos = pos;
				CEntityManager::instance()[eid].LastSent2MePos = pos;
				if(DisplayDebug==1)
					nlinfo("TCP update client %hu a %g %g %g ping %hu", (uint16)eid, pos.x, pos.y, pos.z);
				if(SessionFile) 
					fprintf(SessionFile, "%hu PO %f %f %f\n", (uint16)eid, pos.x, pos.y, pos.z);
				//TODO remove rsxTime param
				bool oc = false;
				if(CEntityManager::instance()[eid].addOpenCloseKey)
				{
					CEntityManager::instance()[eid].addOpenCloseKey = false;
					oc = true;
				}
				CCrashEvent ce(false,CVector::Null);
				if(CEntityManager::instance()[eid].addCrashEventKey.Crash)
				{
					ce = CEntityManager::instance()[eid].addCrashEventKey;
					CEntityManager::instance()[eid].addCrashEventKey.Crash = false;
				}
				CEntityManager::instance()[eid].interpolator().addKey(CEntityInterpolatorKey(CEntityState(pos,oc,ce),rsxTime));
				//CEntityManager::instance()[eid].ping(ping);
			}
			else
			{
				nlstop;
				nlwarning("Received a position of an unknown entity %hu", (uint16)eid);
			}
		}
	}
}*/

/*static void cbUpdateOne(CNetMessage &msgin)
{
	//nlinfo("cbUpdateOne %f",CTimeTask::instance().time());
	float rsxTime = 0.0f;
	uint8 eid;
	CVector pos;
	CVector dpos;
	CVector ddpos;
	//uint16 ping;
	
	// reply to the update first (used for the ping)
	//CNetMessage msgout(CNetMessage::Update);
	//CNetworkTask::instance().send(msgout);
	
	//msgin.serial (rsxTime);
	
	//while(msgin.getPos() < (sint32)msgin.length())
	{
		float deltaCoef = 100;
		//msgin.serial(eid);
		eid = CMtpTarget::instance().controler().getControledEntity();

// 		uint8 dxx,sxx;
// 		msgin.serial(sxx,dxx);
// 		dpos.x = convert8_8fp(sxx,dxx);
// 		uint8 dxy,sxy;
// 		msgin.serial(sxy,dxy);
// 		dpos.y = convert8_8fp(sxy,dxy);
// 		uint8 dxz,sxz;
// 		msgin.serial(sxz,dxz);
// 		dpos.z = convert8_8fp(sxz,dxz);

		uint32 bits;
		msgin.serial(bits);
		packBit32 pb32(bits);
		uint32 ndx,nsx;
		pb32.unpackBits(nsx,6);
		pb32.unpackBits(ndx,4);//z
		//		nlinfo(">>>>z dx/ndx = %d/%d ",dxz,ndx);
		//		nlinfo(">>>>z sx/nsx = %d/%d ",sxz,nsx);
		//nlassert(dxz==ndx);
		//nlassert(sxz==nsx);
		dpos.z = convert8_8fp(nsx,ndx);
		pb32.unpackBits(nsx,6);
		pb32.unpackBits(ndx,4);//y
		//		nlinfo(">>>>y dx/ndx = %d/%d ",dxy,ndx);
		//		nlinfo(">>>>y sx/nsx = %d/%d ",sxy,nsx);
		//nlassert(dxy==ndx);
		//nlassert(sxy==nsx);
		dpos.y = convert8_8fp(nsx,ndx);
		pb32.unpackBits(nsx,6);
		pb32.unpackBits(ndx,4);//x
		//		nlinfo(">>>>x dx/ndx = %d/%d ",dxx,ndx);
		//		nlinfo(">>>>x sx/nsx = %d/%d ",sxx,nsx);
		//nlassert(dxx==ndx);
		//nlassert(sxx==nsx);
		dpos.x = convert8_8fp(nsx,ndx);
		
		//nlinfo(">>>>dx =  %f ",dpos.x - mdx);

		if(CEntityManager::instance().exist(eid))
		{
			pos = CEntityManager::instance()[eid].LastSent2MePos + dpos;
			CEntityManager::instance()[eid].LastSent2MePos = pos;
			if(DisplayDebug==1)
				nlinfo("TCP updateOne client %hu a %g %g %g ping %hu", (uint16)eid, pos.x, pos.y, pos.z);
			if(SessionFile) 
				fprintf(SessionFile, "%hu PO %f %f %f\n", (uint16)eid, pos.x, pos.y, pos.z);
			
			bool oc = false;
			if(CEntityManager::instance()[eid].addOpenCloseKey)
			{
				CEntityManager::instance()[eid].addOpenCloseKey = false;
				oc = true;
			}
			CCrashEvent ce(false,CVector::Null);
			if(CEntityManager::instance()[eid].addCrashEventKey.Crash)
			{
				ce = CEntityManager::instance()[eid].addCrashEventKey;
				CEntityManager::instance()[eid].addCrashEventKey.Crash = false;
			}
			CEntityManager::instance()[eid].interpolator().addKey(CEntityInterpolatorKey(CEntityState(pos,oc,ce),rsxTime));
			//CEntityManager::instance()[eid].ping(ping);
		}
	}
}*/

extern CVector LatestForce;

static void cbFullUpdate(CNetMessage &msgin)
{
	// first, reply to the update (used to compute the ping)
	CNetMessage msgout(CNetMessage::Update);
	uint8 pingnb;
	msgin.serial(pingnb);
	msgout.serial(pingnb);
	if(!CMtpTarget::instance().spectator()) msgout.serial(LatestForce);
	CNetworkTask::instance().send(msgout, 1);
//	nlinfo("*********** send the pong %u %"NL_I64"u", pingnb, CTime::getLocalTime());

	// debug: simulate packet lost when pressing shift
	//if(C3DTask::instance().kbDown(KeySHIFT)) return;

	sint16 packetdt = 0;
	static TTime oldt0 = 0;
	TTime t0;
	msgin.serial (t0);

	if(oldt0	!= 0)
		packetdt = sint16(t0 - oldt0);

	oldt0 = t0;

	if(packetdt <= 0) return;

	static FILE *fp = 0;
	string str1, str2;
	bool first = false;
/*	if(DisplayDebug==3 && !fp)
	{
		fp = fopen("pings.csv", "wt");
		first = true;
	}*/

	bool firstKey = true;
	while(msgin.getPos() < (sint32)msgin.length())
	{
		uint8 eid;
		CVector pos;
		uint16 ping;
		msgin.serial(eid, pos, ping);

		if(CEntityManager::instance().exist(eid))
		{
			CEntity &e = CEntityManager::instance()[eid];

			SessionString += toString("%hu PO %d %d %f %f %f\n", uint16(eid), packetdt, firstKey?1:0, pos.x, pos.y, pos.z);

			bool oc = false;
			if(e.addOpenCloseKey)
			{
				e.addOpenCloseKey = false;
				oc = true;
			}
			CCrashEvent ce(false, CVector::Null);
			if(e.addCrashEventKey.Crash)
			{
				ce = e.addCrashEventKey;
				e.addCrashEventKey.Crash = false;
			}
			e.interpolator().addKey(CEntityInterpolatorKey(CEntityState(pos,oc,ce), float(packetdt)/1000.0f), firstKey);
			e.setPing(ping);
			if(firstKey) firstKey = false;

/*			if(DisplayDebug==3 && fp && first)
			{
				str1 += toString("%s;", e.name().toUtf8().c_str());
			}

			if(DisplayDebug==3 && fp)
			{
				str2 += toString("%hu;", ping);
			}*/

			if(CMtpTarget::instance().controler().Camera.getFollowedEntity() == e.id())
				PingGraph.addOneValue (float(ping));
		}
	}

/*	if(DisplayDebug==3 && fp)
	{
		if(first) fprintf(fp, "%s\n", str1.c_str());
		fprintf(fp, "%s\n", str2.c_str());
	}

	if(DisplayDebug!=3 && fp)
	{
		fclose(fp);
		fp = 0;
	}*/
}


static void cbUpdateList(CNetMessage &msgin)
{
	CEntityManager::instance().updateListId.clear();
	while(msgin.getPos() < (sint32)msgin.length())
	{
		uint8 eid;
		msgin.serial(eid);
		CEntityManager::instance().updateListId.push_back(eid);
	}
}


static void cbEditMode(CNetMessage &msgin)
{
	uint8 editMode;
	msgin.serial(editMode);
	nlinfo("NET: cbEditMode editMode=%hu", (uint16)editMode);

	// TODO: what is the goal of this function???
}

static void cbUpdateElement(CNetMessage &msgin)
{
	uint8 elementType, elementId, selectedBy;
	CVector pos, eulerRot;
	msgin.serial(elementType, elementId, selectedBy, pos, eulerRot);
	nlinfo("NET: cbUpdateElement");
	
	switch(elementType)
	{
	case CEditableElementCommon::Module:
		if(CLevelManager::instance().levelPresent())
			CLevelManager::instance().currentLevel().updateModule(elementId,pos,eulerRot,selectedBy);
		break;
	case CEditableElementCommon::StartPosition:
		if(CLevelManager::instance().levelPresent())
			CLevelManager::instance().currentLevel().updateStartPoint(elementId,pos,eulerRot,selectedBy);
		break;
	default:
		nlwarning("Unknown elemen type %hu", (uint16)elementType);
		break;
	}
}
	
static void cbReady(CNetMessage &msgin)
{
	// called when one player is ready
	uint8 eid;
	msgin.serial(eid);
	//nlinfo("NET: cbReady eid=%hu", (uint16)eid);

	// check if the player exists
	if(!CEntityManager::instance().exist(eid)) { nlwarning("The eid doesn't exist"); return; }
	
	CEntityManager::instance()[eid].setReady(true);
}

static void cbEverybodyReady(CNetMessage &msgin)
{
	//nlinfo("NET: cbEverybodyReady");

	// called when everybody is ready, can start the countdown
	mtpTarget::instance().everybodyReady();
}

static void cbEnableElement(CNetMessage &msgin)
{
	uint8 elementId;
	bool enabled;
	msgin.serial(elementId, enabled);
	nlinfo("NET: cbEnableElement");
	
	if(CLevelManager::instance().levelPresent())
		CLevelManager::instance().currentLevel().module(elementId)->setEnabled(enabled);
}

static void cbDisplayText(CNetMessage &msgin)
{
	float x, y, s;
	CRGBA col;
	double duration;
	ucstring message;
	msgin.serial(x, y, s, message, col, duration);
	//nlinfo("NET: cbDisplayText x=%f y=%f s=%f col=(%d,%d,%d), duration=%f message='%s'", x, y, s, col.R, col.G, col.B, duration, message.toUtf8().c_str());
	message = convertVariableString(message);
	CHudTask::instance().addMessage(CHudMessage(x,y,s,col,duration,message));
}

static void cbStartSession(CNetMessage &msgin)
{
	float timebeforestart, timeout;
	string levelName;
	ucstring str1, str2;
	bool teamMode;
// 	vector<uint8> ranks;
// 	vector<uint8> eids;
	vector<uint8> spectators;

	msgin.serial(timebeforestart, timeout, levelName, teamMode, str1, str2);
  	msgin.serialCont(spectators);
//  	msgin.serialCont(ranks);
//  	msgin.serialCont(eids);

	// bugfix: when loading a team map, the server send the full path instead of the level file name
	levelName = CFile::getFilename(levelName);

	CNetworkTask::instance().sendGoogleAnalytics(toString("/%u/%s/%s", CLoginClientMtp::selectedShard().ShardId, levelName.c_str(), (teamMode?"team":"normal")));

	if(!str1.empty()) str1 += toString("|%.3f", MyRecords[levelName]);

	str1 = convertVariableString(str1);
	str2 = convertVariableString(str2);

/*	nlinfo("NET: cbStartSession timebeforestart=%f timeout=%f levelName='%s' teamMode='%d', str1='%s' str2='%s'",
		timebeforestart, timeout, levelName.c_str(), teamMode, str1.toUtf8().c_str(), str2.toUtf8().c_str());*/

//	nlassert(ranks.size()==eids.size());

// 	for(uint32 i=0;i<eids.size();i++)
// 	{
// 		if(CEntityManager::instance().exist(eids[i]))
// 		{
// 			CEntityManager::instance()[eids[i]].rank(ranks[i]);
// 		}
// 	}

	for(uint i = 0; i < 256; i++)
	{
		if(CEntityManager::instance().entities()[i]->type() != CEntity::Unknown)
		{
			CEntityManager::instance().entities()[i]->setTeam(255);
		}
	}

	CEntityManager::instance().resetSpectators();
	for(uint32 i=0;i<spectators.size();i++)
	{
		if(CEntityManager::instance().exist(spectators[i]))
		{
			CEntityManager::instance()[spectators[i]].setSpectator(true);
		}
	}

	CMtpTarget::instance().startSession(timebeforestart / 1000.0f, timeout / 1000.0f, levelName, teamMode, str1, str2);

	/*
	msgin.serial (mtpTarget::instance().Interface2d.LevelName);
	msgin.serial (mtpTarget::instance().Interface2d.FullLevelName);
	msgin.serial (mtpTarget::instance().Interface2d.string1);
	msgin.serial (mtpTarget::instance().Interface2d.string2);

	mtpTarget::instance().startSession(timebeforestart / 1000, timeout / 1000, mtpTarget::instance().Interface2d.LevelName);
*/
  
}

static void cbSessionState(CNetMessage &msgin)
{
	string sn;
	msgin.serial(sn);
//	nlinfo("NET: cbSessionState sn='%s'", sn.c_str());

	// TODO: what is the goal of this function???
}

static void cbEndSession(CNetMessage &msgin)
{
	//nlinfo("NET: cbEndSession");

	while(msgin.getPos() < (sint32)msgin.length())
	{
		uint8 eid;
		sint32 lastGameScore, totalScore;

		msgin.serial(eid, lastGameScore, totalScore);

		//CEntity *entity = mtpTarget::instance().World.entityById(id);
		//nlassert(entity!=0);
		//entity->setScore(currentScore, totalScore);
		if(CEntityManager::instance().exist(eid))
		{
			// if it's me and my score is 1000 or more, we can now go to the full server, notify the user
			int tutoscore = CConfigFileTask::instance().configFile().getVar("MinTotalScoreToHideTutorial").asInt();
			if(CMtpTarget::instance().controler().getControledEntity()==eid && totalScore > tutoscore && CEntityManager::instance()[eid].totalScore() < tutoscore)
			{
				displayMessage(ucstring("GuiEndTutorial"));
			}

			CEntityManager::instance()[eid].setLastGameScore(lastGameScore);
			CEntityManager::instance()[eid].setTotalScore(totalScore);
		}
	}
	mtpTarget::instance().endSession();

	// create the session replay

	if(!CFile::isDirectory(CConfigFileTask::instance().documentDirectory()+"replay")) CFile::createDirectory(CConfigFileTask::instance().documentDirectory()+"replay");

	if(ReplayFile.empty() && CConfigFileTask::instance().configFile().getVar("GenerateReplay").asInt() == 1)
	{
		int CurrentReplaySavedCount = CConfigFileTask::instance().configFile().getVar("CurrentReplaySavedCount").asInt();
		int MaxReplaySavedCount = CConfigFileTask::instance().configFile().getVar("MaxReplaySavedCount").asInt();
		if(MaxReplaySavedCount < 1) MaxReplaySavedCount = 1;
		if(MaxReplaySavedCount > 999) MaxReplaySavedCount = 999;

		CurrentReplaySavedCount++;
		CurrentReplaySavedCount = CurrentReplaySavedCount % MaxReplaySavedCount;
		CConfigFileTask::instance().configFile().getVar("CurrentReplaySavedCount").setAsInt(CurrentReplaySavedCount);
		CConfigFileTask::instance().configFile().save();

		if(CConfigFileTask::instance().configFile().getVar("LogChat").asInt()==1)
		{
			string fn = toString("%sreplay/session%03d.mtr", CConfigFileTask::instance().documentDirectory().c_str(), CurrentReplaySavedCount);
			FILE *fp = fopen (fn.c_str(), "wt");
			if(fp)
			{
				fwrite(SessionString.c_str(), SessionString.size(), 1, fp);
				fclose(fp);
			}

			if(CMtpTarget::instance().moveReplay())
			{
				string markedReplayDir = CConfigFileTask::instance().documentDirectory()+"replay/marked/";
				if(!CFile::isDirectory(markedReplayDir)) CFile::createDirectory(markedReplayDir);
				string newFilename = markedReplayDir + CFile::getFilename(fn);
				CFile::moveFile(newFilename.c_str(),fn.c_str());
			}
			CMtpTarget::instance().moveReplay(false);
		}
	}
	SessionString.clear();
}

static void cbExecLua(CNetMessage &msgin)
{
	string luaCode;
	msgin.serial(luaCode);
	//nlinfo("NET: cbExecLua luaCode='%s'", luaCode.c_str());

	if(CLevelManager::instance().levelPresent())
		CLevelManager::instance().currentLevel().execLua(luaCode);
}

static void cbCollide(CNetMessage &msgin)
{
	uint8 eid;
	uint8 type;
	CVector pos;
	msgin.serial(eid, type, pos);
	//nlinfo("NET: cbCollide eid=%hu, type=%hu, pos=(%f,%f,%f)", (uint16)eid, (uint16)type, pos.x, pos.y, pos.z);

	// check if the player exists
	if(!CEntityManager::instance().exist(eid)) { nlwarning("The eid doesn't exist"); return; }

	if(CMtpTarget::instance().controler().getControledEntity()==eid)
	{
		// it's my collide when fly
		CExternalCameraTask::instance().setExternalCamera(true);
	}

	switch(type)
	{
	case 1:
		// crash in fly
		CEntityManager::instance()[eid].addCrashEventKey = CCrashEvent(true,pos);
		SessionString += toString("%hu CE %f %f %f\n", (uint16)eid,pos.x,pos.y,pos.z);
	break;
	case 2:
		// crash in water
		CEntityManager::instance()[eid].collideWhenWater();
	break;
	}
}

static void cbTimeArrival(CNetMessage &msgin)
{
	uint8 eid;
	float time;
	msgin.serial(eid, time);
	//nlinfo("NET: cbTimeArrival eid=%hu, time=%f", (uint16)eid, time);

	// check if the player exists
	if(!CEntityManager::instance().exist(eid)) { nlwarning("The eid doesn't exist"); return; }

	CEntityManager::instance().entities()[eid]->ArrivalTime = time;

	if(CMtpTarget::instance().controler().getControledEntity() == eid)
	{
		// it's my arrival
		CExternalCameraTask::instance().setExternalCamera(true);
		CHudTask::instance().addSysMessage(convertVariableString(toString("Landed|%.3f", time)));

		if(CEntityManager::instance().entities()[eid]->team() == 255)
		{
			string fn = CFile::getFilename(CLevelManager::instance().currentLevel().filename());
			if(MyRecords[fn] == 0.0f)
			{
				// first time we play this map, just insert the score
				MyRecords[fn] = time;
			}
			else if(time < MyRecords[fn])
			{
				CChatTask::instance().addLine(convertVariableString(toString("BrokenMyTimeRecord|%.3f|%.3f", time, MyRecords[fn])));
				MyRecords[fn] = time;
			}
		}
	}
}

static void cbLevel(CNetMessage &msgin)
{
	string level;
	msgin.serial(level);

	nlinfo("Receive the level that i don't have, use it");

	string cf = CConfigFileTask::instance().tempDirectory()+"level.lua";
	FILE *fp = fopen(cf.c_str(), "wb");
	if(fp)
	{
		fputs(level.c_str(), fp);
		fclose(fp);
		CLevelManager::instance().loadLevel(cf, false);
	}
}

struct CGuiChallengeEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiChallengeEventBehaviour(bool r) : res(r) { }
	virtual void onPressed()
	{
		CGuiObjectManager::instance().removeFrame("message");
		CNetMessage msgout(CNetMessage::MatchChallenge);
		msgout.serial(res);
		CNetworkTask::instance().send(msgout);
	}
private:
	bool res;
};

static void cbMatchChallenge(CNetMessage &msgin)
{
	ucstring challenger;
	msgin.serial(challenger);

	displayMessage(ucstring("MatchChallenge|")+challenger, new CGuiChallengeEventBehaviour(true), new CGuiChallengeEventBehaviour(false));
}


//
// Callback handler
//

#define SWITCH_CASE(_n) case CNetMessage::_n: { H_AUTO(_n); cb##_n(msgin); } break

void netCallbacksHandler(CNetMessage &msgin)
{
//	nldebug("%"NL_I64"u Received message type %hu size %u", CTime::getLocalTime(), (uint16)msgin.type(), msgin.length());

	switch(msgin.type())
	{
	SWITCH_CASE(Chat);
	SWITCH_CASE(Error);
	SWITCH_CASE(Login);
	SWITCH_CASE(Logout);
	SWITCH_CASE(OpenClose);
	SWITCH_CASE(FullUpdate);
	SWITCH_CASE(UpdateElement);
	SWITCH_CASE(Ready);
	SWITCH_CASE(EverybodyReady);
	SWITCH_CASE(DisplayText);
	SWITCH_CASE(SessionState);
	SWITCH_CASE(StartSession);
	SWITCH_CASE(EndSession);
	SWITCH_CASE(EditMode);
	SWITCH_CASE(EnableElement);
	SWITCH_CASE(ExecLua);
	SWITCH_CASE(Collide);
	SWITCH_CASE(TimeArrival);
	SWITCH_CASE(Level);
	SWITCH_CASE(MatchChallenge);

	default: nlwarning("Received an unknown message type %hu", (uint16)msgin.type()); break;
	}
}
