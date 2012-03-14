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

#include "bot.h"
#include "main.h"
#include "network.h"
#include "variables.h"
#include "lua_engine.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "session_manager.h"
#include "waiting_ready_session_state.h"
#include "waiting_start_session_state.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Functions
//

void CWaitingReadySessionState::update()
{
	if(CSessionManager::getInstance().forceEnding() || CEntityManager::getInstance().everyBodyReady())
	{
		CLevelManager::getInstance().currentLevel().initBeforeStartLevel();

		CEntityManager::EntityConstIt it;
		for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
		{
			if((*it)->type() == CEntity::Client)
			{
				CClient *c = (CClient *)(*it);
				if(c->ReplayFile)
					fclose(c->ReplayFile);
				
				if(!NLMISC::CFile::isDirectory("./replay"))
					NLMISC::CFile::createDirectory("./replay");
				string CurrentLevel = CLevelManager::getInstance().currentLevel().name();
				int maxReplaySavedCount = IService::getInstance()->ConfigFile.getVar("maxReplaySavedCount").asInt();
				if(maxReplaySavedCount<1)
					maxReplaySavedCount=1;
				char maxReplaySavedCountString[8];
				sprintf(maxReplaySavedCountString,"%03d",maxReplaySavedCount-1);
				string maxReplaySavedFileName = "replay/"+CurrentLevel+"."+toString(c->StartingPointId)+"."+maxReplaySavedCountString+".mtr";
				//nlinfo(">>%s",maxReplaySavedFileName.c_str());
				if(!NLMISC::CFile::fileExists(maxReplaySavedFileName))
				{
					//nlinfo(">> max replay count not reach");
					c->ReplayFilename = NLMISC::CFile::findNewFile("replay/"+CurrentLevel+"."+toString(c->StartingPointId)+"..mtr");
					c->ReplayFile = fopen(c->ReplayFilename.c_str(), "wt");
				}
				else
				{
					//nlinfo(">> max replay count reach... skipping record of this replay");
					c->ReplayFile = 0;
				}
			}
			else if((*it)->type() == CEntity::Bot)
			{
				CBot *b = (CBot *)(*it);
				b->loadBotReplay();
			}
		}

		TTime currentTime = CTime::getLocalTime();
		nlinfo("Everybody ready, start session in %g seconds(at %d)", TimeBeforeStart/1000.0f,currentTime+TimeBeforeStart);
		changeState(CWaitingStartSessionState::getInstance());
		CSessionManager::getInstance().startTime(currentTime+(TTime)TimeBeforeStart);
		
		CNetMessage msgout(CNetMessage::EverybodyReady);
		CNetwork::getInstance().send(msgout);
	}
}

