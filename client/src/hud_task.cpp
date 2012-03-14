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

#include "3d_task.h"
#include "hud_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "mtp_target.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "entity_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "resource_manager2.h"
#include "level_manager.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	
void CHudTask::init()
{
	AltimeterMinValue = CConfigFileTask::getInstance().configFile().getVar("AltimeterMinValue").asFloat();
	AltimeterMaxValue = CConfigFileTask::getInstance().configFile().getVar("AltimeterMaxValue").asFloat();
	pressControlMessageAdded = false;
	landClosedMessageAdded = false;
	landClosedMessageAdded2 = false;
}

void CHudTask::update()
{
	// update altimeter value
	uint8 eid = CMtpTarget::getInstance().controler().getControledEntity();
	AltimeterValue = ((eid != 255) ? CEntityManager::getInstance()[eid].interpolator().currentPosition().z : AltimeterMinValue);
	AltimeterValue = min(AltimeterValue, AltimeterMaxValue);
	AltimeterValue = max(AltimeterValue, AltimeterMinValue);
}

void CHudTask::render()
{
	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	
	string str;

	float ptdt = 1.0f;
	bool displaySessionInfo = false;
	float partTime = CMtpTarget::getInstance().timeBeforeSessionStart();
	static uint32 oldPartTime = 0;
	
	if(CMtpTarget::getInstance().State == CMtpTarget::eStartSession)
	{		
		str = "Waiting for other players";
		ptdt = 1.0f;
	}
	if(CMtpTarget::getInstance().State == CMtpTarget::eBeforeFirstSession || CMtpTarget::getInstance().isSpectatorOnly())
	{
		str = "Waiting for a new session";
		ptdt = 1.0f;
	}
	if(CMtpTarget::getInstance().State == CMtpTarget::eReady)
	{
		pressControlMessageAdded = false;
		landClosedMessageAdded = false;
		landClosedMessageAdded2 = false;
		displaySessionInfo = true;
		if (partTime > 5.0f)
			str = "Ready ?";
		else if (partTime > 0.0f)
		{
			uint32 newPartTime = uint32(partTime)+1;

			if(oldPartTime != newPartTime)
			{
				oldPartTime = newPartTime;
				//CSoundManager::instance().playSound(CSoundManager::TSound(CSoundManager::Ready0+newPartTime));
				CSoundManager::instance().playGUISound ("ready" + toString("%u", newPartTime));
			}

			str = toString("%u", ((uint32) partTime)+1);
			ptdt = partTime - (float)(sint)partTime;
		}
		else
			nlstop;
	}
	if(CMtpTarget::getInstance().State == CMtpTarget::eGame)
	{
		if (partTime < 2.0f)
		{
			if(oldPartTime != 0)
			{
				oldPartTime = 0;
				CSoundManager::instance().playGUISound ("ready0");
			}

			str = "Go";
		}
	}
	float scale = 1.0f+3.0f*(1.0f-ptdt);
	CFontManager::getInstance().printf(CRGBA(255, 255, 255, (uint)(ptdt*255)), (C3DTask::getInstance().screenWidth() - str.size() * CFontManager::getInstance().fontWidth()*scale) / 2.0f, 7.0f * CFontManager::getInstance().fontHeight(),scale, str.c_str());

	list<CHudMessage>::iterator it;
	list<CHudMessage>::iterator it2delete;
	for(it=messages.begin();it!=messages.end();)
	{
		CHudMessage m = *it;
		double time = CTimeTask::getInstance().time();
		if(it->endTime!=0.0 && time>it->endTime)
		{
			it2delete = it;
			it++;
			messages.erase(it2delete);
		}
		else
		{
			CFontManager::getInstance().printf(it->col,it->scale * it->x * CFontManager::getInstance().fontWidth(),it->scale * it->y * CFontManager::getInstance().fontHeight(),it->scale, it->message.c_str());			
			it++;
		}
	}
	
	
		if (displaySessionInfo)
		{
			float fontWidth  = (float )CFontManager::getInstance().fontWidth();
			float fontHeight = (float )CFontManager::getInstance().fontHeight();
			string str;
			uint32 len;
			
			if(CLevelManager::getInstance().levelPresent())
				str = "Title: " + CLevelManager::getInstance().currentLevel().name();
			else
				str = "Level not present";
			len = str.size();
			CFontManager::getInstance().printf(CRGBA(255, 255, 255, 255), 1 * fontWidth, 10.0f * fontHeight,1, str.c_str());
			/*
			str = "File : " + CLevelManager::getInstance().currentLevel().filename();
			len = str.size();
			CFontManager::getInstance().printf(CRGBA(255, 255, 255, 255), 1 * fontWidth, 11.0f * fontHeight,1, str.c_str());
			*/
			if(CLevelManager::getInstance().levelPresent())
				str = "Author: " + CLevelManager::getInstance().currentLevel().author();
			else
				str = "Level not present";
			len = str.size();
			CFontManager::getInstance().printf(CRGBA(255, 255, 255, 255), 1 * fontWidth, 12.0f * fontHeight,1, str.c_str());

			CFontManager::getInstance().printf(CRGBA(245, 238, 141, 255), 1 * fontWidth, 16.0f * fontHeight,1, CMtpTarget::getInstance().String1.c_str());
			CFontManager::getInstance().printf(CRGBA(253, 207, 85, 255), 1 * fontWidth, 18.0f * fontHeight,1, CMtpTarget::getInstance().String2.c_str());
			
			/*
			CFontManager::getInstance().printf(CRGBA(245, 238, 141, 255), 1 * fontWidth, 14 * fontHeight,1, "Best score:");
			CFontManager::getInstance().printf(CRGBA(253, 207, 85, 255), 3 * fontWidth, 15 * fontHeight,1, string1.c_str());
			
			CFontManager::getInstance().printf(CRGBA(245, 238, 141, 255), 1 * fontWidth, 16 * fontHeight,1, "Best time:");
			CFontManager::getInstance().printf(CRGBA(253, 207, 85, 255), 3 * fontWidth, 17 * fontHeight,1, string2.c_str());
			*/
		}

	// ace todo put HUD in a task
	
	float baseY = float(C3DTask::getInstance().screenHeight()) - 300.0f;
	float height = 244.0f;
	height = (1-(AltimeterValue - AltimeterMinValue) / (AltimeterMaxValue - AltimeterMinValue)) * height;
	float x1 = float(C3DTask::getInstance().screenWidth() - 40), y1 = baseY, tx1 = 15, ty1 = 8, tw1 = 34-15, th1 = 253-8;
	float x2 = float(C3DTask::getInstance().screenWidth() - 60), y2 = baseY - 18 + height, tx2 = 46, ty2 = 10, tw2 = 78-46, th2 = 43-10;	

	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	CFontManager::getInstance().material().setColor(CRGBA(255, 255, 255, 255));
	
	CQuadUV		quad;
	quad.V0.set(x1,C3DTask::getInstance().screenHeight()-y1,0);
	quad.V1.set(x1,C3DTask::getInstance().screenHeight()-y1-th1,0);
	quad.V2.set(x1+tw1,C3DTask::getInstance().screenHeight()-y1-th1,0);
	quad.V3.set(x1+tw1,C3DTask::getInstance().screenHeight()-y1,0);
	
	int rx1 = (int)tx1;
	int ry1 = (int)ty1;
	int rx2 = (int)tx1+(int)tw1;
	int ry2 = (int)ty1+(int)th1;
	
	quad.Uv0.U= rx1/256.0f;
	quad.Uv0.V= ry1/256.0f;
	quad.Uv1.U= rx1/256.0f;
	quad.Uv1.V= ry2/256.0f;
	quad.Uv2.U= rx2/256.0f;
	quad.Uv2.V= ry2/256.0f;
	quad.Uv3.U= rx2/256.0f;
	quad.Uv3.V= ry1/256.0f;
	
	C3DTask::getInstance().driver().drawQuad (quad, CFontManager::getInstance().material());
	
	quad.V0.set(x2,C3DTask::getInstance().screenHeight()-y2,0);
	quad.V1.set(x2,C3DTask::getInstance().screenHeight()-y2-th2,0);
	quad.V2.set(x2+tw2,C3DTask::getInstance().screenHeight()-y2-th2,0);
	quad.V3.set(x2+tw2,C3DTask::getInstance().screenHeight()-y2,0);
	
	rx1 = (int)tx2;
	ry1 = (int)ty2;
	rx2 = (int)tx2+(int)tw2;
	ry2 = (int)ty2+(int)th2;
	
	quad.Uv0.U= rx1/256.0f;
	quad.Uv0.V= ry1/256.0f;
	quad.Uv1.U= rx1/256.0f;
	quad.Uv1.V= ry2/256.0f;
	quad.Uv2.U= rx2/256.0f;
	quad.Uv2.V= ry2/256.0f;
	quad.Uv3.U= rx2/256.0f;
	quad.Uv3.V= ry1/256.0f;
	
	C3DTask::getInstance().driver().drawQuad (quad, CFontManager::getInstance().material());	
	
	uint8 eid = CMtpTarget::getInstance().controler().Camera.getFollowedEntity();
	if (eid != 255)
	{
		// display our score (bottom right)
		string totalScoreStr = toString("score %d",CEntityManager::getInstance()[eid].totalScore());
		CFontManager::getInstance().printf(CRGBA(245, 238, 141, 255), (float) (C3DTask::getInstance().screenWidth() - totalScoreStr.size() * CFontManager::getInstance().fontWidth() - 10), float(C3DTask::getInstance().screenHeight() - 1 * CFontManager::getInstance().fontHeight()), 1, totalScoreStr.c_str());
		
		if (CEntityManager::getInstance()[eid].interpolator().outOfKey())
			CFontManager::getInstance().printf(CRGBA(255, 0, 0, 255), float(C3DTask::getInstance().screenWidth() / 2 - 70), 100.0f,1, "NET LAG");
	}
	

	double TimeBeforeTimeout = CMtpTarget::getInstance().timeBeforeTimeout();
	if (TimeBeforeTimeout < 0)
		TimeBeforeTimeout = 0;
	string timeBeforeTimeoutStr = toString("Time left %u",(uint)TimeBeforeTimeout);
	CFontManager::getInstance().printf(CRGBA(245, 238, 141, 255), 10.0f, (float)(C3DTask::getInstance().screenHeight() - 20),1,timeBeforeTimeoutStr.c_str() );
	if(!ReplayFile.empty())
	{
		string pressPauseToRestart = "Press pause key to restart replay";
		CFontManager::getInstance().printf(CRGBA(245, 238, 141, 255), 10.0f, (float)(C3DTask::getInstance().screenHeight() - 60),1,pressPauseToRestart.c_str() );
	}
	
	CFontManager::getInstance().printf(CRGBA(255,255,255,255),(C3DTask::getInstance().screenWidth() - _viewedName.size() * CFontManager::getInstance().fontWidth()) / 2.0f,float(C3DTask::getInstance().screenHeight() - 2 * CFontManager::getInstance().fontHeight()),1,_viewedName.c_str());

	if(CMtpTarget::getInstance().State == CMtpTarget::eGame && CMtpTarget::getInstance().displayTutorialInfo())
	{
		if(43<TimeBeforeTimeout && TimeBeforeTimeout<53 && !pressControlMessageAdded)
		{
			pressControlMessageAdded = true;
			addMessage(CHudMessage(5,15,1,string("press control to fly"),CRGBA(255,255,0,255),5));
		}
		if(33<TimeBeforeTimeout && TimeBeforeTimeout<43 && !landClosedMessageAdded)
		{
			landClosedMessageAdded = true;
			addMessage(CHudMessage(5,15,1,string("press control to roll"),CRGBA(255,255,0,255),5));
		}
		if(23<TimeBeforeTimeout && TimeBeforeTimeout<33 && !landClosedMessageAdded2)
		{
			landClosedMessageAdded2 = true;
			addMessage(CHudMessage(0,15,1,string("don't touch anything when you fly"),CRGBA(255,255,0,255),5));
		}
	}
	
	/*
	//updateChat();
	
	bool displayScoreForced = (displaySelected != eDisplayEndSession) && C3DTask::getInstance().kbDown(KeyTAB);
	if (displaySelected == eDisplayEndSession || displayScoreForced)
	{
		C3DTask::getInstance().driver().drawQuad(0.0f, 0.0f, C3DTask::getInstance().screenWidth() , C3DTask::getInstance().screenHeight(), CRGBA (0, 0, 0, 200));
		
		// display all players score
		float x1 = 15.0f;
		float x2 = 250.0f;
		float x3 = 500.0f;
		float x4 = 680.0f;
		float y = 100.0f;
		CFontManager::getInstance().printf(CRGBA(245, 238, 141), x1, y, 1, "name");
		CFontManager::getInstance().printf(CRGBA(245, 238, 141), x2, y, 1, "score");
		CFontManager::getInstance().printf(CRGBA(245, 238, 141), x3, y, 1, "total");
		CFontManager::getInstance().printf(CRGBA(245, 238, 141), x4, y, 1, "ping");
		y += fontHeight+10;
		
		vector<uint8> eids;
		CEntityManager::getInstance().getEIdSortedByScore(eids);
		
		for(uint i = 0; i < eids.size(); i++, y += fontHeight)
		{
			CFontManager::getInstance().printf(CEntityManager::getInstance()[i].color(), x1, y, 1, "%s%s", CEntityManager::getInstance()[i].name().c_str(), (CEntityManager::getInstance()[i].spectator()?" :spec:":""));
			if (!displayScoreForced)
				CFontManager::getInstance().printf(CEntityManager::getInstance()[i].color(), x2, y, 1, "%u", CEntityManager::getInstance()[i].currentScore());
			CFontManager::getInstance().printf(CEntityManager::getInstance()[i].color(), x3, y, 1, "%u", CEntityManager::getInstance()[i].totalScore());
			CFontManager::getInstance().printf(CEntityManager::getInstance()[i].color(), x4, y, 1, "%u", CEntityManager::getInstance()[i].ping());
		}
	}	
	*/
}

void CHudTask::release()
{
}

void CHudTask::setDisplayViewedName(const string &name)
{
	_viewedName = name;
}

void CHudTask::addSysMessage(const string &txt)
{
	CHudMessage msg(-1.0f, 0.0f, 1.0f, txt, CRGBA::White, 10.0);
	addMessage(msg);
}

void CHudTask::addMessage(const CHudMessage &newm)
{
	list<CHudMessage>::iterator it;
	for(it=messages.begin();it!=messages.end();)
	{
		CHudMessage m = *it;
		if(m.x == newm.x && m.y==newm.y)
		{
			list<CHudMessage>::iterator it2delete = it;
			it++;
			messages.erase(it2delete);
		}
		else
		{
			it++;
		}
	}

	messages.push_back(newm);
}

