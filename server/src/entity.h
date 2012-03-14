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

#ifndef MTPT_ENTITY
#define MTPT_ENTITY


//
// Includes
//

#include <set>
#include <queue>
#include <vector>
#include <ode/ode.h>

#include <nel/misc/value_smoother.h>
#include <nel/misc/rgba.h>
#include <nel/misc/time_nl.h>

#include <nel/net/tcp_sock.h>

#include "module.h"
#include "../../common/lua_utility.h"

class CEntityProxy;

//
// Functions
//

class CEntity
{
public:

	enum TEntity { Unknown, Client, Bot };

	CEntity(uint8 eid);
	CEntity(uint8 eid, const std::string &name);
	virtual ~CEntity();

	void _luaInit();
	void init(std::string &name,std::string &texture, NLMISC::CRGBA &color, std::string &trace, std::string &meshName);
 
	// switch the open close status of the entity
	// return true if the switch really occurs
	virtual bool openClose();
	virtual bool isOpen();
	virtual void isOpen(bool oc);
	
	virtual void update();
	virtual void reset();
	virtual void initBeforeStartLevel();
	
	virtual void display(NLMISC::CLog &log = *NLMISC::InfoLog) { }

	virtual void setForce(const NLMISC::CVector &clientForce);

	void maxLinearVelocity(float maxLinVel);
	float maxLinearVelocity();
	float meanVelocity();

	float defaultAccel();
	void defaultAccel(float a);
	void enableOpenCloseCommand(bool e);
	void enableCrashInFly(bool e);
	
	// accessors

	uint8					 id() const { return Id; }
	const std::string		&name() const { return Name; }
	void name(const std::string &name);
	void startPointId(uint8 id);
	bool isAdmin() const;
	bool isModerator() const;
	bool canSpeak();
	void canSpeak(bool speak);
	std::string team();
	
	void position(NLMISC::CVector pos);
	NLMISC::CVector  position() {return Pos;};
	
	TEntity					 type() const { return Type; }

	bool					 spectator() const { return Spectator; }
	void					 spectator(bool b) { Spectator = b; }

	virtual bool			forceReceived() = 0;
	
	CEntityProxy		*luaProxy;

	std::set<CModule *> collideModules;
	std::set<CEntity *> collideEntity;
	bool                collideWater;
	bool                callOpenCloseLua;

	void voteMap(const std::string &voteMap);
	std::string voteMap();
public:

	// ugly public variables

	// entity customization
	NLMISC::CRGBA		 Color;
	std::string			 Texture;
	std::string			 Trace;
	std::string			 MeshName;

	sint32				 CurrentScore;		// the current score of the current session
	sint32				 Score;				// the total score off all session
	float				 ArrivalTime;				// time in second before total stop
	NLMISC::CVector		 Pos;				// position of the client
	NLMISC::CVector		 Force;				// the current client force received

	bool				 OpenClose;			// open = true, close = false (thx to cahanta)
	uint32				 NbOpenClose;		// how many time did the client press space key
	uint32				 MaxOpenClose;		// how many time did the client can press space key
	dBodyID				 Body;
	dGeomID				 Geom;
	bool				 FreezeCommand;
	bool				 SendCollideWhenFly;
	NLMISC::CVector      CollideWhenFlyPos;
	
	NLMISC::CValueSmootherTemplate<uint16> Ping;
	std::queue<std::pair<uint8, NLMISC::TTime> > LastSentPing;

	bool				 Spectator;

	bool				 InGame;
	bool				 Ready;
	bool				 WaitingReady;
	NLMISC::TTime		 WaitingReadyTimeoutStart;
	//	bool jointed;
	float				 Accel;
	float				 Friction;

	float				 LastVel[10];		// last velocity
	uint32				 LastVelPos;		// between 0 and 9
	uint8				 StartingPointId;
	NLMISC::CVector		 LastSent2MePos;
	NLMISC::CVector		 LastSent2OthersPos;
	sint8				 LastSentSX;
	uint32				 networkVersion;
	
	bool				 OnTheWater;

	// just display the state of the last physic render (just for watch purpose)
	std::string			 ColState;

	// lua
	bool				 LuaInit;
	float				 DefaultAccel;
	float				 DefaultFriction;
	bool				 EnableOpenCloseCommand;
	bool				 EnableCrashInFly;
	bool				 ForceReceived;
	uint32				 AfkCount;
	std::string			 VoteMap;
	
	// store all ping for
	std::vector<std::pair<NLMISC::TTime, uint32> >	StatPing;
	
private:

	void init ();

	TEntity				 Type;				// type of entity
	uint8				 Id;
	std::string			 Name;				// empty name mean that the client not already send login information
	float				 MaxLinearVelocity;
	bool				 CanSpeak;
};

#endif
