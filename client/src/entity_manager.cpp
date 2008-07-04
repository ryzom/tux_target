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

#include <nel/misc/command.h>

#include "main.h"
#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "entity_manager.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CEntityManager::CEntityManager() : ReloadTextures(string("ReloadTexture"))
{
	entities().clear();
	for(uint i = 0; i < 256; i++)
	{
		entities().push_back(new CEntity);
	}
	mat = C3DTask::instance().createMaterial();
}

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
	{
		CSynchronized<list<pair<string, uint8> > >::CAccessor acces(&ReloadTextures);
		if(!acces.value().empty())
		{
			for(list<pair<string, uint8> >::iterator it = acces.value().begin(); it != acces.value().end(); it++)
			{
				if(exist((*it).second))
				{
					getById((*it).second)->setTexture((*it).first);
				}
			}
			acces.value().clear();
		}
	}

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

void CEntityManager::add(uint8 eid, const ucstring &name, sint32 totalScore, CRGBA &color, const string &texture, bool spectator, bool isLocal, const string &trace, const string &meshName, bool fullversion)
{
//	nlinfo("CEntityManager::add(%d:%s)",eid,name.toUtf8().c_str());
	nlassert(!exist(eid));
	entities()[eid]->init(CEntity::Player, name, totalScore, color, texture, meshName, spectator, isLocal, trace, fullversion);
}

void CEntityManager::remove(uint8 eid)
{
	//nlinfo("CEntityManager::remove(%d)",eid);
	nlassert(exist(eid));
	if(CMtpTarget::instance().controler().Camera.getFollowedEntity() == eid)
		CMtpTarget::instance().resetFollowedEntity();

	entities()[eid]->reset();
}

bool CEntityManager::exist(uint8 eid)
{
	return eid!=255 && entities()[eid]->Type != CEntity::Unknown;
}

CEntity &CEntityManager::operator [](uint8 eid)
{
	nlassert(exist(eid));
	return *entities()[eid]; //todo prevent external code to access entities without using accessor
}

static bool entitySort(const uint8 &left, const uint8 &right)
{
	CEntity *el = CEntityManager::instance().getById(left);
	CEntity *er = CEntityManager::instance().getById(right);
	if(el->lastGameScore() == er->lastGameScore())
		return el->totalScore() > er->totalScore();
	else
		return el->lastGameScore() > er->lastGameScore();
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

	sort(eids.begin(), eids.end(), entitySort);
}

uint8 CEntityManager::findFirstEId()
{
	return findNextEId(255);
}

uint8 CEntityManager::findNextEId(uint8 eid)
{
	uint8 neid = eid;
	while(++neid != eid)
		if(entities()[neid]->type() != CEntity::Unknown && !entities()[neid]->spectator())
			break;
	return neid;
}

uint8 CEntityManager::findPreviousEId(uint8 eid)
{
	uint8 neid = eid;
	while(--neid != eid)
		if(entities()[neid]->type() != CEntity::Unknown && !entities()[neid]->spectator())
			break;
	return neid;
}

void CEntityManager::startSession(bool firstSession)
{
	if(!firstSession)
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

void CEntityManager::resetSpectators()
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->setSpectator(false);
		}
	}
}

void CEntityManager::everybodyReady(bool state)
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->setReady(state);
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

void CEntityManager::renderNames(bool all)
{
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			entities()[i]->renderName(all);
		}
	}
}

void CEntityManager::render()
{
	if(DisplayDebug!=4) return;

	mat.setZWrite(true);
	mat.setBlend(true);
	mat.setBlendFunc(UMaterial::srcalpha, UMaterial::invsrcalpha);

	C3DTask::instance().driver().setFrustum(C3DTask::instance().scene().getCam().getFrustum());
	UCamera cam = C3DTask::instance().scene().getCam();
	C3DTask::instance().driver().setMatrixMode3D(cam);

	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
		{
			CRGBA col(i*20,(uint8)(uintptr_t)(void*)entities()[i],128,200);
			deque<CEntityInterpolatorKey>::const_iterator key1;
			deque<CEntityInterpolatorKey>::const_iterator key2;
			const deque<CEntityInterpolatorKey> *keys = entities()[i]->interpolator().keys(key1, key2);

			if(keys->size()>=2)
			{
				for(deque<CEntityInterpolatorKey>::const_iterator k2 = keys->begin()+1; k2 != keys->end(); k2++)
				{
					deque<CEntityInterpolatorKey>::const_iterator k1 = k2-1;
					mat.setColor(col);
					CLine line;
					line.V0 = (*k1).value().Position;
					line.V1 = (*k2).value().Position;
					C3DTask::instance().driver().drawLine(line, mat);

					float s;
					if(k1 == key1) { mat.setColor(CRGBA(255,0,0)); s = 0.025f; }
					else if(k1 == key2) { mat.setColor(CRGBA(0,255,0)); s = 0.025f; }
					else { mat.setColor(CRGBA(255,255,255)); s = 0.001f; }
					line.V1 = (*k1).value().Position+CVector(0, 0, s);
					C3DTask::instance().driver().drawLine(line, mat);
				}
			}
		}
	}
}

void CEntityManager::load3d()
{
return;
	for(uint i = 0; i < 256; i++)
	{
		if(entities()[i]->type() != CEntity::Unknown)
				entities()[i]->load3d();
	}
}

CEntity *CEntityManager::getByName(ucstring &name)
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

void CEntityManager::release()
{
	for(uint i = 0; i < 256; i++)
	{
		if(exist(i))
		{
			remove(i);
		}
	}
}

//
// Commands
//

NLMISC_COMMAND(pl, "display the player list", "")
{
	string str;
	for(uint i = 0; i < 256; i++)
	{
		if(CEntityManager::instance().entities()[i]->type() != CEntity::Unknown)
		{
			str += toString("#%d %s ", CEntityManager::instance().entities()[i]->id(), CEntityManager::instance().entities()[i]->name().toUtf8().c_str());
		}
	}
	log.displayNL(str.c_str());
	return true;
}
