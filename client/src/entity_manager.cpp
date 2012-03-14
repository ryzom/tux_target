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

#include "main.h"
#include "time_task.h"
#include "mtp_target.h"
#include "entity_manager.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
CEntityManager::CEntityManager()
{
	entities().clear();
	for(uint i = 0; i < 256; i++)
	{
		entities().push_back(new CEntity);
	}
}

/*
void CEntityManager::add(uint8 eid)
{
	uint tid = getThreadId();
	nlassert(tid==TaskManagerThreadId || tid==NetworkThreadId);
	if(tid==TaskManagerThreadId)
		ClientToAddTaskManagerThread.push_back(eid);
	else
		ClientToAddNetworkThread.push_back(eid);
}

void CEntityManager::remove(uint8 eid)
{
	uint tid = getThreadId();
	nlassert(tid==TaskManagerThreadId || tid==NetworkThreadId);
	if(tid==TaskManagerThreadId)
		ClientToRemoveTaskManagerThread.push_back(eid);
	else
		ClientToRemoveNetworkThread.push_back(eid);
	
}
*/

void CEntityManager::init()
{
	for(uint i = 0; i < 256; i++)
	{
		entities()[i]->id(i);
		entities()[i]->reset();
	}
	updateListId.clear();
}

void CEntityManager::update()
{
	// do some security check
	for(uint i = 0; i < 256; i++)
	{
		nlassert(entities()[i]->Id == i);
	}
	nlassert(entities()[255]->Type == CEntity::Unknown);

	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
			entities()[i]->update();
	}
}

void CEntityManager::add(uint8 eid, const std::string &name, sint32 totalScore, CRGBA &color, const string &texture, bool spectator, bool isLocal, const string &trace, const string &meshName)
{
	nlinfo("CEntityManager::add(%d:%s)",eid,name.c_str());
	nlassert(!exist(eid));
	entities()[eid]->init(CEntity::Player, name, totalScore, color, texture, meshName, spectator, isLocal, trace);
}

void CEntityManager::remove(uint8 eid)
{
	nlinfo("CEntityManager::remove(%d)",eid);
	nlassert(exist(eid));
	if(CMtpTarget::getInstance().controler().Camera.getFollowedEntity() == eid)
		CMtpTarget::getInstance().controler().Camera.setFollowedEntity(255);
	
	entities()[eid]->reset();
}

bool CEntityManager::exist(uint8 eid)
{
	nlassert(eid != 255);
	return entities()[eid]->Type != CEntity::Unknown;
}

CEntity &CEntityManager::operator [](uint8 eid)
{
	nlassert(exist(eid));
	return *entities()[eid]; //todo prevent external code to access entites without using accessor
}

void CEntityManager::getEIdSortedByScore(vector<uint8> &eids) 
{
	eids.clear();

	for (uint i = 0; i < 255; i++)
	{
		if (entities()[i]->type() == CEntity::Unknown)
			continue;

		eids.push_back(i);
	}
}

uint8 CEntityManager::findFirstEId() 
{
	return findNextEId(255);
}


uint8 CEntityManager::findNextEId(uint8 eid) 
{
	uint8 neid = eid;
	while(++neid != eid)
		if(entities()[neid]->type() != CEntity::Unknown)
			return neid;
	return eid;
}

uint8 CEntityManager::findPreviousEId(uint8 eid) 
{
	uint8 neid = eid;
	while(--neid != eid)
		if(entities()[neid]->type() != CEntity::Unknown)
			return neid;
	return 255;
}

void CEntityManager::startSession()
{
	if(!CMtpTarget::getInstance().isSpectatorOnly()) //don't close everybody if we start session being spectator
	{
		for(uint i = 0; i < 256; i++)
		{
			if(entities()[i]->type() != CEntity::Unknown)
			{
				entities()[i]->close();
				entities()[i]->sessionStart();
			}
		}
	}
}

void CEntityManager::sessionReset()
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->sessionReset();
		}
	}
}

void CEntityManager::luaInit()
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->luaInit();
		}
	}
}

void CEntityManager::resetSpectator()
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->spectator(false);
		}
	}
}

void CEntityManager::everybodyReady(bool state)
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->ready(state);
		}
	}
}

uint8 CEntityManager::size()
{
	uint8 nb = 0;
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			nb++;
		}
	}
	return nb;
}

void CEntityManager::renderNames()
{
	uint renderCount = 0;
	CVector pos;
	/*
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			bool res = entities()[i]->namePosOnScreen(pos);
			if(res)
				renderCount++;
		}
	}	

	if(renderCount<3)
		for(uint i = 0; i < 256; i++)
	*/
		for(uint i = 0; i < 256 && renderCount<=3; i++)
		{
			if(entities()[i]->type() != CEntity::Unknown)
			{
				bool res = entities()[i]->namePosOnScreen(pos);
				/*
				if(res)
					renderCount++;
				*/
				entities()[i]->renderName();
			}
		}	
}

void CEntityManager::render()
{
}

void CEntityManager::load3d()
{
	for(uint i = 0; i < 256; i++)
	{
		//skeet
		//if(entities()[i]->type() != CEntity::Unknown && !entities()[i]->spectator())
		if(entities()[i]->type() != CEntity::Unknown)
				entities()[i]->load3d();
	}
}

CEntity *CEntityManager::getByName(string &name)
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown && entities()[i]->name()==name)
			return entities()[i];
	}	
	return 0;
}

CEntity *CEntityManager::getById(uint8 eid)
{
	if(!exist(eid))
		return NULL;
	return entities()[eid];
}


//
// Commands
//
