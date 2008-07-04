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

#include <nel/misc/path.h>
#include <nel/misc/debug.h>
#include <nel/misc/report.h>
#include <nel/misc/config_file.h>

#include <nel/3d/u_scene.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_text_context.h>

#include <nel/net/email.h>

#include "main.h"
#include "level.h"
#include "entity.h"
#include "3d_task.h"
#include "gui_task.h"
#include "hud_task.h"
#include "time_task.h"
#include "game_task.h"
#include "mtp_target.h"
#include "intro_task.h"
#include "editor_task.h"
#include "network_task.h"
#include "task_manager.h"
#include "font_manager.h"
#include "swap_3d_task.h"
#include "sound_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "entity_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "external_camera_task.h"
#include "../../common/constant.h"
#include "../../common/async_job.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace NLNET;


//
// Variables
//

//extern FILE *SessionFile;
string SessionString;	// contains all the info about the session that will be saved

string ReleaseVersion("1.5.");	// append the last number with the current_version file
uint16 CurrentVersion = 0; // the content of the file current_version
uint16 LatestVersion = 0; // the content of the file latest_version from the server

const char *mtpTargetConfigFilename = "mtp_target.cfg";
bool FullVersion = false;

uint8 FundRaised;
ucstring FundGoal;


//
// Functions
//

// Order
//
// 0->100		low level task
// 100->1000	3d render
// 1000->10000	hud render
// 10000		3d swap
//

void CMtpTarget::init()
{
	FirstSession = true;
	MoveReplay = false;
	Error = false;
	DoError = false;

	// setup default task
	//nlinfo("Setting up default tasks");
	CTaskManager::instance().add(CConfigFileTask::instance(), 10);
	CTaskManager::instance().add(CTimeTask::instance(), 20);
	CTaskManager::instance().add(C3DTask::instance(), 30);
	CTaskManager::instance().add(CEntityManager::instance(), 40);
	CTaskManager::instance().add(CNetworkTask::instance(), 50);
	CAsyncJob::instance().init();
	CTaskManager::instance().add(CExternalCameraTask::instance(), 115);
	CTaskManager::instance().add(CGuiTask::instance(), 1050);
	CTaskManager::instance().add(CEditorTask::instance(), 120);
	CTaskManager::instance().add(CSwap3DTask::instance(), 10000);
	CTaskManager::instance().add(CFontManager::instance(), 40);

	CTaskManager::instance().add(CSoundManager::instance(), 55);
	//nlinfo("Default tasks set");

	Controler = new CControler;

	if (!ReplayFile.empty())
	{
		string levelName = loadReplayFile();

		//CMtpTarget::instance().State = CMtpTarget::eReady;
		CTaskManager::instance().add(CGameTask::instance(), 5);
		CMtpTarget::instance().startSession(1, 60, levelName, false, ucstring(""), ucstring(""));
	}
	else
	{
		// start with intro task
		CTaskManager::instance().add(CIntroTask::instance(), 60);
		reset();
	}
}

void CMtpTarget::error(const string &reason)
{
	//FastExit = true;
	DoError = true;
	ErrorReason = reason;
	Error = true;
	CNetworkTask::instance().stop();
}

void CMtpTarget::reset()
{
	nlinfo("CMtpTarget::reset()");
	CEntityManager::instance().sessionReset();
	controler().reset();
	NewSession = false;
	TimeBeforeSessionStart = 0.0f;
	TimeBeforeTimeout = 0.0f;
	State = CMtpTarget::eBeforeFirstSession;
	DisplayTutorialInfo = false;
}

bool CMtpTarget::error()
{
	return Error;
}

void CMtpTarget::_error()
{
	nlinfo("error occurred : stop all and reset");
	reset();
	CGameTask::instance().stop();
	CEditorTask::instance().stop();
	CEditorTask::instance().enable(false);
	CBackgroundTask::instance().restart();
	//CIntroTask::instance().reset();
	::error(ErrorReason);
	CIntroTask::instance().restart();
	CLevelManager::instance().release();
	DoError = false;
}

void CMtpTarget::update()
{
	if(DoError)
		_error();

 	if(NewSession)
 		loadNewSession(false);

	controler().update();

	if(FollowEntity)
	{
		C3DTask::instance().scene().getCam().setMatrix(*CMtpTarget::instance().controler().Camera.getMatrixFollow());
	}

	if(CMtpTarget::instance().State == CMtpTarget::eReady)
	{
		TimeBeforeSessionStart -= (float)CTimeTask::instance().deltaTime();

		if(TimeBeforeSessionStart > 4.0f)
		{
			CMtpTarget::instance().controler().Camera.reset();
			CMtpTarget::instance().controler().Camera.ForcedSpeed = 0.0f;
		}
		else
			CMtpTarget::instance().controler().Camera.ForcedSpeed = 1.0f;

		if(TimeBeforeSessionStart <= 0)
		{
			TimeBeforeSessionStart = 0;
			CMtpTarget::instance().State = CMtpTarget::eGame;
			CEntityManager::instance().sessionReset();
		}
	}
	if(CMtpTarget::instance().State == CMtpTarget::eGame)
	{
		//CMtpTarget::instance().controler().Camera.reset();
		TimeBeforeSessionStart += (float)CTimeTask::instance().deltaTime();
		TimeBeforeTimeout -= (float)CTimeTask::instance().deltaTime();
	}
	if(CMtpTarget::instance().State == CMtpTarget::eBeforeFirstSession)
	{
		//CMtpTarget::instance().controler().Camera.reset();
		TimeBeforeSessionStart += (float)CTimeTask::instance().deltaTime();
		TimeBeforeTimeout -= (float)CTimeTask::instance().deltaTime();
	}
	if(CMtpTarget::instance().State == CMtpTarget::eStartSession && spectator())
	{
		//CMtpTarget::instance().controler().Camera.reset();
		TimeBeforeSessionStart += (float)CTimeTask::instance().deltaTime();
		TimeBeforeTimeout -= (float)CTimeTask::instance().deltaTime();
	}

	const CMatrix &cameraMatrix = C3DTask::instance().scene().getCam().getMatrix();
	CSoundManager::instance().updateListener(cameraMatrix.getPos(), CVector::Null, cameraMatrix.getJ(), cameraMatrix.getK());
}

void CMtpTarget::displayTutorialInfo(bool b)
{
	DisplayTutorialInfo = b;
}

bool CMtpTarget::displayTutorialInfo()
{
	return DisplayTutorialInfo;
}

void CMtpTarget::loadNewSession(bool firstSession)
{
	nlassert(NewSession);
	CEntityManager::instance().everybodyReady(false);

	NewSession = false;
	nlinfo("LEVEL: loadNewSession() '%s'", NewLevelName.c_str());

	if(NewLevelName.empty())
	{
		nlwarning("NewLevelName is empty");
		return;
	}

	if(TimeBeforeSessionStart != 0.0f)
	{
		// i'm in the new session and all other entities, reset spectator
		//CEntityManager::instance().resetSpectators();

		if(!CEditorTask::instance().enable())
			FollowEntity = true;
	}
	else
	{
		FollowEntity = false;
	}

	CEntityManager::instance().load3d();

	CLevelManager::instance().loadLevel(NewLevelName);

	CEntityManager::instance().startSession(firstSession);

	CMtpTarget::instance().controler().Camera.reset();
	/*
	if (CMtpTarget::instance().controler().getControledEntity() != 255)
		CEntityManager::instance()[CMtpTarget::instance().controler().getControledEntity()].interpolator().reset();
	*/
	resetFollowedEntity();
	controler().Camera.reset();

	//if(CLevelManager::instance().levelPresent())
	//	CLevelManager::instance().currentLevel().reset();
	//else
	//	nlwarning("there is no level");

	State = eStartSession;

	nlinfo ("LEVEL: level loaded");

	// say to server that we are ready if other players wait for us
	CNetworkTask::instance().ready();

	// now we wait the message that say that everybody is ready

	// create the session replay
/*
	if(!NLMISC::CFile::isDirectory("replay"))
		NLMISC::CFile::createDirectory("replay");

	if(ReplayFile.empty() && CConfigFileTask::instance().configFile().getVar("GenerateReplay").asInt() == 1)
	{
		int CurrentReplaySavedCount = CConfigFileTask::instance().configFile().getVar("currentReplaySavedCount").asInt();
		int maxReplaySavedCount = CConfigFileTask::instance().configFile().getVar("maxReplaySavedCount").asInt();
		if(maxReplaySavedCount < 1) maxReplaySavedCount = 1;
		if(maxReplaySavedCount > 999) maxReplaySavedCount = 999;

		currentReplaySavedCount++;
		currentReplaySavedCount = currentReplaySavedCount % maxReplaySavedCount;
		CConfigFileTask::instance().configFile().getVar("currentReplaySavedCount").setAsInt(currentReplaySavedCount);
		CConfigFileTask::instance().configFile().save();

		SessionFileName = toString("replay/session%03d.mtr",currentReplaySavedCount);
 		//SessionFile = fopen (SessionFileName.c_str(), "wt");
 		//nlassert (SessionFile != 0);
	}*/

	SessionString.clear();
	SessionString += toString("%hu %s %f\n", (uint16)CEntityManager::instance().size(), NewLevelName.c_str(), TimeBeforeTimeout+TimeBeforeSessionStart);

	uint8 eid = CEntityManager::instance().findFirstEId();
	if (eid != 255)
	{
		do
		{
			int self = (eid == CMtpTarget::instance().controler().getControledEntity())?1:0;
			ucstring name = CEntityManager::instance()[eid].name();
			for(uint i = 0; i < name.size(); i++) if(name[i] == ucchar(' ')) name[i] = ucchar('_');
			SessionString += toString("%hu %s %d %d %d '%s'\n", (uint16)eid, name.toUtf8().c_str(), self, CEntityManager::instance()[eid].currentScore(), CEntityManager::instance()[eid].totalScore(),CEntityManager::instance()[eid].texture().c_str());
			eid = CEntityManager::instance().findNextEId(eid);
		}
		while(eid != CEntityManager::instance().findFirstEId());
	}

	if(!ReplayFile.empty())
		mtpTarget::instance().everybodyReady();

}

void CMtpTarget::startSession(float timeBeforeSessionStart, float timeBeforeTimeout, const string &levelName, bool teamMode, const ucstring &str1, const ucstring &str2, bool firstSession)
{
	nlinfo("LEVEL: level '%s' loaded, it timeouts in %g seconds", levelName.c_str(), timeBeforeTimeout);

	C3DTask::instance().mouseListener().reset();
	if(firstSession)
	{
		uint8 eid = CMtpTarget::instance().controler().getControledEntity();
		if(eid != 255)
		{
			CEntityManager::instance()[eid].setSpectator(true);
		}
	}
	FirstSession = firstSession;
	TimeBeforeSessionStart = timeBeforeSessionStart;
	TimeBeforeTimeout = timeBeforeTimeout;
	NewLevelName = levelName;
	TeamMode = teamMode;
	String1 = str1;
	String2 = str2;

	// load the level now or team color is not working when you are the first player
	NewSession = true;
	loadNewSession(firstSession);
}

void CMtpTarget::resetFollowedEntity()
{
	if(spectator()) return;
	uint8 eid = CMtpTarget::instance().controler().getControledEntity();
	if(!CEntityManager::instance().exist(eid)) eid = 255;
	CMtpTarget::instance().controler().Camera.setFollowedEntity(eid);
	CMtpTarget::instance().controler().ViewedEId = eid;
}

void CMtpTarget::moveReplay(bool b)
{
	MoveReplay = b;
}

bool CMtpTarget::moveReplay(void)
{
	return MoveReplay;
}

void mtpTarget::everybodyReady()
{
	// everybody is ok, let s count down
	CMtpTarget::instance().State = CMtpTarget::eReady;
	CEntityManager::instance().everybodyReady(true);
}

void mtpTarget::endSession()
{
	// end of a session
	CMtpTarget::instance().State = CMtpTarget::eEndSession;
	//CExternalCameraTask::instance().setExternalCamera(false);
	CHudTask::instance().clearMessages();
	CEntityManager::instance().sessionReset();
}

string CMtpTarget::loadReplayFile()
{
	nlinfo("Loading replay file : %s", ReplayFile.c_str());
	string levelName;
	FILE *fp = fopen (ReplayFile.c_str(), "rt");
	if (fp != 0)
	{
		uint nbplayer, eid, self, currentScore, totalScore;
		char name[100];
		char texture[100];

		while(CEntityManager::instance().size() != 0)
		{
			CEntityManager::instance().remove(CEntityManager::instance().findFirstEId());
		}
		resetInterpolator();

		fscanf (fp, "%d %s %f", &nbplayer, &name, &TimeBeforeTimeout);
		levelName = name;

		// add players
		for(uint i = 0; i < nbplayer; i++)
		{
			strcpy(texture,"");
			// TODO manage trace and mesh in replay
			fscanf (fp, "%d %s %d %d %d %s", &eid, &name, &self, &currentScore, &totalScore, &texture);

			CRGBA col(255,255,255);
			string textureName = texture;
			string traceName = "trace";
			string meshName = "pingoo";
			textureName = textureName.substr(1,textureName.size()-2);
			ucstring ucname;
			ucname.fromUtf8(name);
			CEntityManager::instance().add(eid, ucname, totalScore, col, textureName, false,self!=0, traceName, meshName, false);
		}

		char cmd[10];
		sint32 dt = 0;
		float x, y, z;
		sint32 first;
		while (!feof (fp))
		{
			fscanf (fp, "%d %s", &eid, cmd);
			if (string(cmd) == "PO")
			{
				fscanf (fp, "%d %d %f %f %f", &dt, &first, &x, &y, &z);

				if (CEntityManager::instance().exist(eid))
				{
					CVector v(x, y, z);
					//CEntityManager::instance()[eid].interpolator().addKey(CEntityInterpolatorKey(CEntityState(v,false),t));//.position(v,t, false); //put the entity in the good position
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
					CEntityManager::instance()[eid].interpolator().addKey(CEntityInterpolatorKey(CEntityState(v,oc,ce,CEntityManager::instance()[eid].addChatLine), float(dt)/1000.0f), first==1);
					CEntityManager::instance()[eid].addChatLine.clear();
				}
				else
					nlwarning ("%d not found", eid);
			}
			else if (string(cmd) == "OC")
			{
				if(CEntityManager::instance().exist(eid))
					CEntityManager::instance()[eid].addOpenCloseKey = true;
			}
			else if (string(cmd) == "CE")
			{
				fscanf (fp, "%f %f %f", &x, &y, &z);
				CVector v(x, y, z);
				if(CEntityManager::instance().exist(eid))
					CEntityManager::instance()[eid].addCrashEventKey = CCrashEvent(true,v);
			}
			else if (string(cmd) == "CH")
			{
				char chatLine[1024];
				fgets(chatLine,1024,fp);
				uint l = strlen(chatLine);
				if(l>0 && chatLine[l-1]=='\n')
					chatLine[l-1] = '\0';
				uint8 eid = CEntityManager::instance().findFirstEId();
				if(eid!=255)
				{
					if(CEntityManager::instance().exist(eid))
						CEntityManager::instance()[eid].addChatLine = ucstring::makeFromUtf8(chatLine);
				}
				nlinfo("replay chat line : %s",chatLine);
			}
			else
			{
				nlwarning ("Unknown command %s for user %d", cmd, eid);
			}
		}

		fclose (fp);
	}
	else
		nlwarning("cannot open replay file");

	return levelName;
}

bool CMtpTarget::spectator()
{
	uint8 eid = CMtpTarget::instance().controler().getControledEntity();
	if(eid != 255)
	{
		return CEntityManager::instance()[eid].spectator();
	}
	else
	{
		return false;
	}
}

ucstring convertVariableString(const ucstring &str)
{
	vector<string> res;
	explode(str.toUtf8(), string("|"), res, true);
	if(res.empty() || !CI18N::hasTranslation(res[0])) return str;

	ucstring s = CI18N::get(res[0]);
	for(uint i = 1; i < res.size(); i++)
	{
		size_t p = 0;
		p = s.find('$', p);
		if(p == string::npos) break;
		s.replace(p, 1, ucstring::makeFromUtf8(res[i]));
		p += ucstring::makeFromUtf8(res[i]).size();
	}
	return s;
}
