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

#include "level.h"
#include "3d_task.h"
#include "hud_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "mtp_target.h"
#include "login_client.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "external_camera_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

void CHudTask::init()
{
	ViewedEId = 255;
	AltimeterMinValue = CConfigFileTask::instance().configFile().getVar("AltimeterMinValue").asFloat();
	AltimeterMaxValue = CConfigFileTask::instance().configFile().getVar("AltimeterMaxValue").asFloat();
	AltimeterValue = 0.0f;
}

void CHudTask::update()
{
	AltimeterMinValue = CConfigFileTask::instance().configFile().getVar("AltimeterMinValue").asFloat();
	AltimeterMaxValue = CConfigFileTask::instance().configFile().getVar("AltimeterMaxValue").asFloat();

	// update altimeter value
	uint8 eid = CMtpTarget::instance().controler().getControledEntity();
	AltimeterValue = ((eid != 255) ? CEntityManager::instance()[eid].interpolator().position().z : AltimeterMinValue);
	AltimeterValue = min(AltimeterValue, AltimeterMaxValue);
	AltimeterValue = max(AltimeterValue, AltimeterMinValue);
}

void CHudTask::render()
{
	if(CConfigFileTask::instance().configFile().getVar("NoHUD").asInt()==1) return;

	C3DTask::instance().driver().setFrustum(CFrustum(0, (float)C3DTask::instance().screenWidth(), 0, (float)C3DTask::instance().screenHeight(), -1, 1, false));

	ucstring str2;

	float ptdt = 1.0f;
	bool displaySessionInfo = false;
	float partTime = CMtpTarget::instance().timeBeforeSessionStart();
	static uint32 oldPartTime = 0;

	if(CMtpTarget::instance().State == CMtpTarget::eStartSession)
	{
		CExternalCameraTask::instance().setExternalCamera(true, false);
		str2 = CI18N::get("WaitPlayers");
		ptdt = 1.0f;
	}
	if(CMtpTarget::instance().State == CMtpTarget::eBeforeFirstSession || CMtpTarget::instance().spectator())
	{
		if(CMtpTarget::instance().firstSession())
			str2 = CI18N::get("WaitSession");
		ptdt = 1.0f;
	}
	if(CMtpTarget::instance().State == CMtpTarget::eReady)
	{
		CExternalCameraTask::instance().setExternalCamera(true, false);
		displaySessionInfo = true;

		if (partTime > 4.0f)
		{
			str2 = CI18N::get("CountDownReady");
		}
		else if (partTime > 0.0f)
		{
			uint32 newPartTime = uint32(partTime)+1;

			if(oldPartTime != newPartTime)
			{
				oldPartTime = newPartTime;
				CSoundManager::instance().playSound(CSoundManager::TSound(CSoundManager::Ready0+newPartTime));
			}

			str2 = ucstring(toString("%u", newPartTime));
			ptdt = partTime - (float)(sint)partTime;
		}
		else
			nlstop;
	}
	if(CMtpTarget::instance().State == CMtpTarget::eGame)
	{
		if (partTime < 2.0f)
		{
			if(oldPartTime != 0)
			{
				oldPartTime = 0;
				CSoundManager::instance().playSound(CSoundManager::TSound(CSoundManager::Ready0));
			}
			str2 = CI18N::get("CountDownGo");
			CExternalCameraTask::instance().setExternalCamera(false);
		}
	}

	float scale = 1.0f+3.0f*(1.0f-ptdt);

	CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::MiddleBottom);
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 255, 255, (uint)(ptdt*255)), CFontManager::instance().screenCenterX(), 7.0f, str2);
	CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::BottomLeft);

	float stackedY = float(CConfigFileTask::instance().configFile().getVar("ChatLineCount").asInt()+1);

	for(list<CHudMessage>::iterator it=Messages.begin();it!=Messages.end();)
	{
		CHudMessage m = *it;
		double time = CTimeTask::instance().time();
		if(it->endTime!=0.0 && time>it->endTime)
		{
			list<CHudMessage>::iterator it2delete = it;
			it++;
			Messages.erase(it2delete);
		}
		else
		{
			it++;
		}
	}

	for(list<CHudMessage>::reverse_iterator it = Messages.rbegin(); it != Messages.rend(); it++)
	{
		if(it->x == -1.0f)
		{
			CFontManager::instance().print(CFontManager::TCChat, 0.0f, (float)stackedY++, it->message);
		}
		else
		{
			float x = (it->x == -2.0f)? CFontManager::instance().screenCenterX() : it->x;

			CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(it->hotSpot);
			CFontManager::instance().print(CFontManager::TCBig, it->col, x, it->y, it->message);
			CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::BottomLeft);
		}
	}

	float fs = float(CFontManager::instance().textContext(CFontManager::TCBig).getFontSize());

	if (displaySessionInfo)
	{
		float y = 150.0f/fs;
		float x1 = 10.0f/fs;

		if(CLevelManager::instance().levelPresent())
		{
			CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::MiddleBottom);
			CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 200, 0), CFontManager::instance().screenCenterX(), 5.0f, CLevelManager::instance().currentLevel().info());
			CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::BottomLeft);

			ucstring str = CI18N::get("Level") + ": " + CLevelManager::instance().currentLevel().name();
			if(CMtpTarget::instance().TeamMode) str += " " + CI18N::get("InTeam");
			str += " (" + CFile::getFilenameWithoutExtension(CLevelManager::instance().currentLevel().filename()) + ")";
			CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 255, 255), x1, y++, str);
		}

		CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x1, y++, CMtpTarget::instance().String1);
		//CFontManager::instance().print(CFontManager::TCBig, CRGBA(253, 207, 85), x1, y++, CMtpTarget::instance().String2);
		//if(CLevelManager::instance().levelPresent())
		//	CFontManager::instance().print(CFontManager::TCBig, CRGBA(253, 207, 85), x1, y++, ucstring(toString("My Record: %.2f", MyRecords[CFile::getFilename(CLevelManager::instance().currentLevel().filename())])));
	}

	if(CMtpTarget::instance().spectator() && CLevelManager::instance().levelPresent()/* && !CMtpTarget::instance().firstSession()*/)
	{
		CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::MiddleBottom);
		ucstring str1;
		ucstring str2 = CI18N::get("F9F10ToFollow");
		if(CLevelManager::instance().currentLevel().advancedLevel())
			str1 = CI18N::get("BuyToPlay");
		float py = float((C3DTask::instance().screenHeight() - 3.5*fs)/fs);
		CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 0, 0), CFontManager::instance().screenCenterX(), py--, str1);
		CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 128, 128), CFontManager::instance().screenCenterX(), py--, str2);
		CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::BottomLeft);
	}

	// display the arrow and gauge of the altitude
	float baseY = C3DTask::instance().screenHeight() - 300.0f;
	float height = 244.0f;
	height = (1-(AltimeterValue - AltimeterMinValue) / (AltimeterMaxValue - AltimeterMinValue)) * height;
	float x1 = float(C3DTask::instance().screenWidth() - 40), y1 = baseY + 220.0f;
	float x2 = float(C3DTask::instance().screenWidth() - 55), y2 = baseY - 18 + height;
	float size = float(CFontManager::instance().textContext(CFontManager::TCChat).getFontSize());
	CFontManager::instance().print(CFontManager::TCChat, x1/size, y1/size, ucstring(":hud_gauge:"), true);
	CFontManager::instance().print(CFontManager::TCChat, x2/size, y2/size, ucstring(":hud_arrow:"), true);

	float lfs = (float)CFontManager::instance().textContext(CFontManager::TCDebug).getFontSize();

	ucstring altStr = CI18N::get("Altitude") + toString(": %.0f", AltimeterValue*10.0f);
	CFontManager::instance().print(CFontManager::TCDebug, CRGBA(255, 255, 255, 255), (float)(C3DTask::instance().screenWidth())/lfs - 5.5f, float(C3DTask::instance().screenHeight())/lfs - 4.5f, altStr);

	uint8 eid = CMtpTarget::instance().controler().Camera.getFollowedEntity();

	if (eid != 255)
	{
 		float speed = CEntityManager::instance()[eid].interpolator().smoothSpeed()/GScale;
 		ucstring speedStr = CI18N::get("Speed") + toString(": %.0f", speed);
 		CFontManager::instance().print(CFontManager::TCDebug, CRGBA(255, 255, 255, 255), (float)(C3DTask::instance().screenWidth())/lfs - 5.5f, float(C3DTask::instance().screenHeight())/lfs - 3.5f, speedStr);

		// display our score (bottom right)
		ucstring scoreStr = CI18N::get("Score") + toString(": %d", CEntityManager::instance()[eid].currentScore());
		CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141, 255), float(C3DTask::instance().screenWidth()-150)/fs, float(C3DTask::instance().screenHeight() - 1.5f*fs)/fs, scoreStr);

		/* if (CEntityManager::instance()[eid].interpolator().outOfKey())
		{
			float px = (C3DTask::instance().screenWidth()/2.0f - 7.0f*fs/2.0f)/fs;
			CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 0, 0, 255), px, 10.0f, ucstring("NET LAG"));
		} */
	}

	double TimeBeforeTimeout = CMtpTarget::instance().timeBeforeTimeout();
	if (TimeBeforeTimeout < 0) TimeBeforeTimeout = 0;
	ucstring timeBeforeTimeoutStr = CI18N::get("TimeLeft")+toString(": %u",(uint)TimeBeforeTimeout);
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), 1.0f, (float)(C3DTask::instance().screenHeight() - 1.5*fs)/fs, timeBeforeTimeoutStr);

	if(!ReplayFile.empty())
		CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), 1.0f, (float)(C3DTask::instance().screenHeight() - 2.5*fs)/fs, CI18N::get("HomeToRestart"));

	if(CEntityManager::instance().exist(ViewedEId))
	{
		CRGBA col;
		switch(CEntityManager::instance()[ViewedEId].team())
		{
		case 0: col.set(255,0,0); break;
		case 1: col.set(128,128,255); break;
		default: col.set(255,255,255); break;
		}
		CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::MiddleBottom);
		CFontManager::instance().print(CFontManager::TCBig, col, CFontManager::instance().screenCenterX(), float(C3DTask::instance().screenHeight() - 1.5*fs)/fs, CEntityManager::instance()[ViewedEId].name());
		CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::BottomLeft);
	}

	displayTutorial();
}

void CHudTask::addSysMessage(const ucstring &txt)
{
	CHudMessage msg(-1.0f, 0.0f, 1.0f, CRGBA::White, 10.0, txt);
	addMessage(msg);
}

void CHudTask::addMessage(const CHudMessage &newm)
{
	if(newm.x != -1.0f)
	{
		for(list<CHudMessage>::iterator it=Messages.begin();it!=Messages.end();)
		{
			CHudMessage &m = *it;
			if(m.x == newm.x && m.y==newm.y)
			{
				list<CHudMessage>::iterator it2delete = it;
				it++;
				Messages.erase(it2delete);
			}
			else
			{
				it++;
			}
		}
	}
	Messages.push_back(newm);
}

void CHudTask::clearMessages()
{
	// only clear non stacked messages
	list<CHudMessage>::iterator it;
	list<CHudMessage>::iterator it2delete;
	for(it=Messages.begin();it!=Messages.end();)
	{
		CHudMessage &m = *it;
		if(m.x != -1.0f)
		{
			it2delete = it;
			it++;
			Messages.erase(it2delete);
		}
		else
		{
			it++;
		}
	}
}

void CHudTask::displayTutorial()
{
	if(!CMtpTarget::instance().displayTutorialInfo()) return;

	double TimeBeforeTimeout = CMtpTarget::instance().timeBeforeTimeout();

	static bool msg1, msg2, msg3, msg4;

	if(CMtpTarget::instance().State == CMtpTarget::eReady)
	{
		msg1 = msg2 = msg3 = msg4 = false;
	}

	ucstring msg;

	if(CMtpTarget::instance().State == CMtpTarget::eGame)
	{
		if(TimeBeforeTimeout<63 && !msg1)
		{
			msg1 = true;
			msg = CI18N::get("TutoUp");
		}
		if(TimeBeforeTimeout<53 && !msg2)
		{
			msg2 = true;
			msg = CI18N::get("TutoCTRL");
		}
		if(TimeBeforeTimeout<46 && !msg3)
		{
			msg3 = true;
			msg = CI18N::get("TutoArrow");
		}
		if(TimeBeforeTimeout<38 && !msg4)
		{
			msg4 = true;
			msg = CI18N::get("TutoCTRL2");
		}
		if(!msg.empty()) addMessage(CHudMessage(-2.0f, 6, 1, CRGBA(255,255,0), 6, msg, UTextContext::MiddleBottom));
	}
}
