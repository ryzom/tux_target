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
#include "client.h"
#include "network.h"
#include "variables.h"
#include "lua_engine.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "session_manager.h"
#include "running_session_state.h"
#include "waiting_start_session_state.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Functions
//

void CWaitingStartSessionState::update()
{
	TTime currentTime = CTime::getLocalTime();
	
	if(CSessionManager::getInstance().forceEnding() || currentTime > CSessionManager::getInstance().startTime())
	{
		CEntityManager::EntityConstIt it;
		for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
		{
			(*it)->InGame = true;
		}
		changeState(CRunningSessionState::getInstance());
		// set gravity if the game started
		dWorldSetGravity(World, 0.0f, 0.0f, Gravity);
		nlinfo("set gravity to %f", Gravity);
	}
}
