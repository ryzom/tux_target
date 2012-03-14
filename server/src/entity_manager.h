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

#ifndef MTPT_ENTITY_MANAGER
#define MTPT_ENTITY_MANAGER


//
// Includes
//

#include <nel/misc/reader_writer.h>
#include <nel/misc/variable.h>
#include <nel/misc/singleton.h>

#include "entity.h"
#include "client.h"


//
// Variables
//

extern NLMISC::CVariable<bool> SavePingStat;


//
// Classes
//

class CEntityManager : public NLMISC::CSingleton<CEntityManager>
{
public:

	typedef NLMISC::CRWSynchronized <std::list <CEntity*> >	CEntities;
	typedef std::list <CEntity*>::iterator					EntityIt;
	typedef std::list <CEntity*>::const_iterator			EntityConstIt;	

	void init();
	void update();
	void release();
	void reset();
	
	std::list <CEntity*> &entities() { return Entities; }

	std::string check(const std::string &login, const std::string &password, bool dontCheck, sint32 &score);

	// this add is to add a client
	void addClient(NLNET::TSockId sock);
		
//	void add(const std::string &name, NLNET::CTcpSock *sock=0, const std::string &cookie="", uint32 score=0, bool isBot=false, bool isAutomaticBot=false);

	void login(CEntity *c);

	void add(CEntity *entity);
	void remove(uint8 eid);
	void remove(const std::string &name);
	CEntity *getByName(const std::string &name);
	
	void addBot(const std::string &name, bool isAutomatic);
	void removeBot();

	void initBeforeStartLevel();
	void openClose(uint8 eid);
	void update(uint8 eid, const NLMISC::CVector &entityForce);

	uint8 nbEntities();

	uint getTeam(uint8 eid,uint teamCount);
	
	bool connected(const std::string &name);
	uint humanClientCount();

	bool nameExist(std::string name);
	void saveAllValidReplay();
	bool everyBodyReady();
	CEntity *getById(uint8 eid);
	CEntity *getByString(std::string ident);
	void checkAfkClient();
		
	void displayText(float x,float y, float scale, NLMISC::CRGBA col, double duration, const std::string &text);
	void displayText(uint8 eid, float x,float y, float scale, NLMISC::CRGBA col, double duration, const std::string &text);
		
	float slowerVelocity();
		
	CEntity *getNthEntity(uint8 number);
	
	bool isAdmin(const std::string &name) const;
	bool isModerator(const std::string &name) const;

	
	std::list<uint8> IdUpdateList;
	
private:
	
	std::list <CEntity*> Entities;

	uint8		findNewId();
	void		checkForcedClientCount();
	void		updateIdUpdateList();

};

#endif
