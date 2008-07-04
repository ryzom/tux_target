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

#ifndef MT_ENTITY_MANAGER_H
#define MT_ENTITY_MANAGER_H

//
// This class manages all entities using a 256 entries static array.
// Using that static array to be able to be multithread safe.
// The entry 255 is always empty because 255 is the code for "unknown" entity.
//


//
// Includes
//

#include <nel/misc/reader_writer.h>

#include <nel/3d/u_material.h>

#include "entity.h"


//
// Classes
//

class CEntityManager : public NLMISC::CSingleton<CEntityManager>, public ITask
{
public:
	typedef vector <CEntity *> CEntities;
	
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();
		
	void renderNames(bool all = false);
	virtual string name() const { return "CEntityManager"; }

	void	add(uint8 eid, const ucstring &name, sint32 totalScore, NLMISC::CRGBA &color, const string &texture, bool spectator, bool isLocal, const string &trace, const string &meshName, bool fullversion);
	void	remove(uint8 eid);

	bool	exist(uint8 eid) ;

	void	startSession(bool firstSession);
	void	sessionReset();
		
	// set all spectator to no more spectator
	void	resetSpectators();
	void	everybodyReady(bool state);
		
	void	load3d();

	CEntity &operator [](uint8 eid);
	CEntity *getByName(ucstring &name);
	CEntity *getById(uint8 eid);
	
	void	getEIdSortedByScore(vector<uint8> &eids);

	uint8	findFirstEId();
	uint8	findNextEId(uint8 eid);
	uint8	findPreviousEId(uint8 eid);

	uint8	size();

	list <uint8> updateListId;
	CEntities &entities() { return Entities; }

	friend class NLMISC::CSingleton<CEntityManager>;

	// Contains the list of entities the manager have to reload the texture
	NLMISC::CSynchronized<list<pair<string, uint8> > > ReloadTextures;

private:

	CEntityManager();

	// Used to display entities paths
	NL3D::UMaterial mat;

	CEntities Entities;
};

#endif
