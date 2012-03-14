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

#include "physics.h"
#include "bot.h"
#include "main.h"
#include "network.h"
#include "variables.h"
#include "lua_engine.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "session_manager.h"
#include "waiting_ready_session_state.h"
#include "waiting_clients_session_state.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Functions
//

void CWaitingClientsSessionState::update()
{
	uint humanClientCount = CEntityManager::getInstance().humanClientCount();

	if(humanClientCount >= NbWaitingClients)
	{
		CEntityManager::EntityConstIt it;
		// ok to launch a session
		changeState(CWaitingReadySessionState::getInstance());

		string string1, string2;
//ace		mtLevelManager::newLevel(CurrentLevel, string1, string2);

		CLevelManager::getInstance().newLevel(string1, string2);
		string CurrentLevel = CLevelManager::getInstance().currentLevel().fileName();

//ace		string luaCodeFilename = CPath::lookup(CurrentLevel+".lua", false);
//		CLuaEngine::loadLevelCode(luaCodeFilename);


		// reset everybody in startpos
		nlinfo("CWaitingClientsSessionState ends : reset every body");
		static uint st = 0;
		uint i=st++;
		for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++, i++)
		{
			//dGeomSetPosition((*it)->Geom, StartX+2.5*i, StartY, StartZ);
			//dBodySetPosition((*it)->Body, StartX+2.5*i, StartY, StartZ);
			
			CVector startPos;
			CLevelManager::getInstance().currentLevel().nextStartingPoint(startPos, (*it)->StartingPointId);

			//i = i % StartPointList.size();
			//(*it)->StartingPointId = i;
			//CVector startPos = StartPointList[i];

			dBodySetPosition((*it)->Body, startPos.x, startPos.y, startPos.z);
			dGeomSetPosition((*it)->Geom, startPos.x, startPos.y, startPos.z);
			
			dBodySetLinearVel((*it)->Body, 0.0f, 0.0f, 0.0f);
			dBodySetAngularVel((*it)->Body, 0.0f, 0.0f, 0.0f);
			(*it)->Force = CVector::Null;
			(*it)->OpenClose = false;
			(*it)->FreezeCommand = false;
			(*it)->SendCollideWhenFly = false;
			(*it)->NbOpenClose = 0;
			(*it)->InGame = false;
			(*it)->Ready = false;
			(*it)->WaitingReady = true;
			(*it)->WaitingReadyTimeoutStart = CTime::getLocalTime();
			(*it)->ArrivalTime = 0.0f;
			(*it)->OnTheWater = false;
			(*it)->spectator(false);

			for(uint i = 0; i < 10; i++)
				(*it)->LastVel[i] = 0;
			
			(*it)->LastVelPos = 0;
			//(*it)->initBeforeStartLevel();
			
			// bot are always ready
			if((*it)->type() == CEntity::Bot)
			{
				(*it)->Ready = true;
				(*it)->WaitingReady = false;
			}
		}
		
		nlinfo("sending StartSession to everybody");
		CNetMessage msgout(CNetMessage::StartSession);
		msgout.serial(TimeBeforeStart);
		float ttimeout = (float)CLevelManager::getInstance().timeTimeout();
		msgout.serial(ttimeout);
		msgout.serial(CurrentLevel, string1, string2);
		//ace		msgout.serial(CurrentLevel, LevelName, string1, string2, Author);
		vector<uint8> ranks;
		vector<uint8> eids;
		for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
		{
			CEntity *e = *it;
			ranks.push_back(e->StartingPointId);
			eids.push_back(e->id());
			nlinfo("startpoint(%d,%s) = %d",e->id(),e->name().c_str(),e->StartingPointId);
		}
		msgout.serialCont(ranks);		
		msgout.serialCont(eids);		
		CNetwork::getInstance().send(msgout);
		
		for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++, i++)
		{
			if((*it)->type() == CEntity::Bot)
			{
				CNetMessage msgout(CNetMessage::Ready);
				uint8 eid = (*it)->id();
				msgout.serial(eid);
				CNetwork::getInstance().send(msgout);
			}
		}

/*
		for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
		{
			CLuaEngine::clientInit(*it);
		}
*/

//		CollisionEntityListIt eit;
//		for(eit = CollisionEntityList.begin(); eit != CollisionEntityList.end(); eit++)
//		{
//			CCollisionEntity *entity = &(*eit);
//			CLuaEngine::collisionEntityInit(entity);
//		}

		dWorldSetGravity(World, 0.0f, 0.0f, 0.0f);
		nlinfo("set gravity : off");
		CSessionManager::getInstance().forceEnding(false);
	}
	else
	{
		if(_restart)
		{
			CNetwork::getInstance().sendToPublicChat("quit restart required");
			nlinfo("server restart required");
			throw "server restart required";
		}
		if(firstUpdate)
		{
			CLevelManager::getInstance().release();
		}
	}
	firstUpdate = false;
}

NLMISC_COMMAND(restart, "restart server when human player count = 0", "")
{
	CWaitingClientsSessionState::getInstance().restart();
	return true;
}
