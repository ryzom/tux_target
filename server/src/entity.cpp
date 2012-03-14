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

#include <nel/misc/path.h>

#include "main.h"
#include "entity.h"
#include "network.h"
#include "physics.h"
#include "variables.h"
#include "session_manager.h"
#include "level_manager.h"
#include "entity_lua_proxy.h"
#include "lua_engine.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Types
//



//
// Declarations
//



//
// Variables
//


//
// Functions
//

void CEntity::init ()
{
	Type = Unknown;
	Id = 255;
	CurrentScore = 0;
	Score = 0;
	ArrivalTime = 0.0f;
	OpenClose = false;
	NbOpenClose = 0;
	MaxOpenClose = (uint32)DefaultMaxOpenClose;
	FreezeCommand = true;
	SendCollideWhenFly = false;
	CollideWhenFlyPos = CVector(0,0,0);
	InGame = false;
	Ready = false;
	WaitingReady = false;
	EnableOpenCloseCommand = true;
	EnableCrashInFly = true;
	LastVelPos = 0;
	StartingPointId = 255;
	OnTheWater = false;
	Accel = 0.0f;
	DefaultAccel = 0;
	DefaultFriction = 0;
	LastSent2MePos = CVector::Null;
	LastSent2OthersPos = CVector::Null;
	LastSentSX = 0;
	Friction = 0.0f;
	LuaInit = false;
	Pos = CVector(Id,0,0);//CVector::Null;
	Force = CVector::Null;
	for(uint i = 0; i < 10; i++) LastVel[i] = 0.0f;
	Spectator = true;
	MaxLinearVelocity = 0;
	ForceReceived = true;
	AfkCount = 0;
	VoteMap = "";
	CanSpeak = true;

	nlassert(World);
	pausePhysics();
	
	// create body
	Body = dBodyCreate(World);
	nlassert(Body);

	// set default position
	dBodySetPosition(Body, Pos.x, Pos.y, Pos.z);

	// set default angle
	dMatrix3 rot;
	dRFromAxisAndAngle(rot, dRandReal()*2.0f-1.0f, dRandReal()*2.0f-1.0f, dRandReal()*2.0f-1.0f, dRandReal()*10.0f-5.0f);
	dBodySetRotation(Body, rot);

	dMass m;
	dReal radius = 0.01f;
	dMassSetSphere(&m, SphereDensity, radius);
    dBodySetMass(Body, &m);
	{
		CSynchronized<dSpaceID>::CAccessor acces(&Space);
		Geom = dCreateSphere(acces.value(), radius);
	}
	nlassert(Geom);

	dGeomSetCategoryBits(Geom,MT_CLIENT_SPECTRUM);	// this is a client
	dGeomSetCollideBits(Geom,MT_CLIENT_SPECTRUM|MT_SCENE_SPECTRUM); // this client collide with other clients and with the scene
	dGeomSetData(Geom, (void *)this);
	
	dGeomSetBody(Geom, Body);
	luaProxy = 0;
	resumePhysics();
}


CEntity::CEntity(uint8 eid) : Ping(50)
{
	nlassert(eid!=255);
	init();
	Id = eid;
	Type = Client;
	_luaInit();
}

CEntity::CEntity(uint8 eid, const std::string &name) : Ping(50)
{
	nlassert(eid!=255);
	init();
	Id = eid;
	Name = name;
	Type = Bot;
	Spectator = false;
	_luaInit();
}

void CEntity::init(std::string &name,std::string &texture, NLMISC::CRGBA &color,std::string &trace,std::string &meshName)
{
	this->name(name);
	this->Color = color;
	this->Texture = texture;
	this->Trace = trace;
	this->MeshName = meshName;
	
	ForceReceived = true;
	AfkCount = 0;
	VoteMap = "";
}

void CEntity::_luaInit()
{
	luaProxy = new CEntityProxy(CLuaEngine::getInstance().session(),this);
}


CEntity::~CEntity()
{
	
	if(luaProxy)
		delete luaProxy;
	luaProxy = 0;

	pausePhysics();
	if(Geom)
	{
		// to be sure, set the data to dummy before erasing
		dGeomSetData(Geom, (void *)0xDEADBEEF);
		dGeomDestroy(Geom);
		Geom = 0;
	}

	if(Body)
	{
		dBodyDestroy(Body);
		Body = 0;
	}
	resumePhysics();
}

bool CEntity::isAdmin() const
{
	CConfigFile::CVar &admin = IService::getInstance()->ConfigFile.getVar("Admin");
	for(uint i = 0; i < (uint)admin.size(); i++)
	{
		if(admin.asString(i) == name())
		{
			return true;
		}
	}
	return false;
}

bool CEntity::isModerator() const
{
	CConfigFile::CVar &moderator = IService::getInstance()->ConfigFile.getVar("Moderator");
	for(uint i = 0; i < (uint)moderator.size(); i++)
	{
		if(moderator.asString(i) == name())
		{
			return true;
		}
	}
	return false;
}

void CEntity::name(const string &name)
{
	Name = name;
}


void CEntity::startPointId(uint8 id)
{
	StartingPointId = id % CLevelManager::getInstance().currentLevel().getStartPointCount();
	CVector startPos = CLevelManager::getInstance().currentLevel().getStartPoint(StartingPointId)->position();
	pausePhysics();
	dBodySetPosition(Body, startPos.x, startPos.y, startPos.z);
	dGeomSetPosition(Geom, startPos.x, startPos.y, startPos.z);
	resumePhysics();
	if(type()==CEntity::Client)
		if(CNetwork::getInstance().version()>=2)
		{
			string code = "";
			code += "getEntityById(";
			code += toString(Id);
			code += "):setStartPointId(";
			code += toString(StartingPointId);
			code += ");";
			CNetMessage msgout(CNetMessage::ExecLua);
			msgout.serial(code);
			CNetwork::getInstance().send(msgout);
		}
		
}

void CEntity::reset() 
{
	collideModules.clear();
	collideEntity.clear();
	collideWater = false;
	callOpenCloseLua = false;
	MaxLinearVelocity = 0;
	MaxOpenClose = (uint32)DefaultMaxOpenClose;
	dMass m;
	dReal radius = 0.01f;
	pausePhysics();
	dMassSetSphere(&m, SphereDensity, radius);
    dBodySetMass(Body, &m);
	resumePhysics();
	DefaultAccel = 0;
	DefaultFriction = 0;
	MaxLinearVelocity = 0;
	EnableOpenCloseCommand = true;
	EnableCrashInFly = true;
	SendCollideWhenFly = false;
	CollideWhenFlyPos = CVector(0,0,0);
	ForceReceived = true;
	StatPing.clear();
}

void CEntity::update() 
{
	H_AUTO(CEntityUpdate);

	if(spectator())
	{
		collideModules.clear();
		collideEntity.clear();
		collideWater = false;
		callOpenCloseLua = false;
		return;
	}
	
	if(luaProxy)
		luaProxy->call("preUpdate");

	if(SendCollideWhenFly)
	{
		SendCollideWhenFly = false;
		if(CNetwork::getInstance().version()>=2)
		{
			CNetMessage msgout(CNetMessage::CollideWhenFly);
			uint8 eid = id();
			msgout.serial(eid);
			msgout.serial(CollideWhenFlyPos);
			CNetwork::getInstance().send(msgout);
		}
	}

	set<CModule *>::iterator mit;
	for(mit=collideModules.begin();mit!=collideModules.end();mit++)
		CLuaEngine::getInstance().entitySceneCollideEvent(this,*mit);	
	collideModules.clear();

	set<CEntity *>::iterator eit;
	for(eit=collideEntity.begin();eit!=collideEntity.end();eit++)
		CLuaEngine::getInstance().entityEntityCollideEvent(this,*eit);	
	collideEntity.clear();

	if(collideWater)
	{
		CLuaEngine::getInstance().entityWaterCollideEvent(this);	
		collideWater = false;
	}

	if(callOpenCloseLua)
	{
		if(luaProxy)
			luaProxy->call("onOpenCloseSwitch");
		callOpenCloseLua = false;
	}
	
	if(luaProxy)
		luaProxy->call("update");
}

void CEntity::initBeforeStartLevel()
{
	ForceReceived = false;
	if(luaProxy)
		luaProxy->call("init");
}

bool CEntity::isOpen()
{
	return OpenClose;
}

void CEntity::isOpen(bool open)
{
	OpenClose = open;
}

bool CEntity::openClose()
{
	if(!EnableOpenCloseCommand)
		return false;

	if(FreezeCommand)
	{
		nlinfo("Client '%s' tries to %s but he is frozen", Name.c_str(), (OpenClose?"open":"close"));
		return false;
	}

	if(NbOpenClose >= MaxOpenClose)
	{
		nlinfo("Client '%s' tries to %s but he reached the MaxOpenClose limit (%d/%d)", Name.c_str(), (OpenClose?"open":"close"),NbOpenClose,MaxOpenClose);
		return false;
	}

	OpenClose = !OpenClose;
	NbOpenClose++;

	pausePhysics();
	if(NbOpenClose >= MaxOpenClose)
	{
		// no more openclose, reset movement accel and vel
		//dBodySetLinearVel(Body, 0.0f, 0.0f, 0.0f);
		dBodySetAngularVel(Body, 0.0f, 0.0f, 0.0f);
		Force = CVector::Null;
	}

	nlinfo("Client '%s' is now %s", Name.c_str(), (OpenClose?"open":"close"));

	// reset all physics values
	if(OpenClose)
	{
		const dReal *vel = dBodyGetLinearVel(Body);
		dBodySetLinearVel(Body, vel[0], vel[1], 0.0f);
		dBodySetAngularVel(Body, 0.0f, 0.0f, 0.0f);
		dMatrix3 R;
		dRSetIdentity(R);
		dBodySetRotation(Body, R);
	}
	resumePhysics();
	
	callOpenCloseLua = true;
	
	return true;
}


void CEntity::setForce(const CVector &clientForce)
{
	// ignoring client update if not in session
	if(CSessionManager::getInstance().endTime() != 0 || CSessionManager::getInstance().startTime() == 0 || CTime::getLocalTime() < CSessionManager::getInstance().startTime())
	{
		//nlinfo("can't update user, no session");
		return;
	}
	
	// can't move because no more open close left
	if(NbOpenClose >= MaxOpenClose)
	{
		Force = CVector::Null;
		//nlinfo("can't update user, max open close reached");
		return;
	}
	
	// client can't move anymore
	if(FreezeCommand)
	{
		//nlinfo("can't update user, freezed");
		return;
	}
	
	Force = clientForce;
	
	if(OpenClose)
	{
		//ForceX *= OpenAccelCoef;
		//ForceY *= OpenAccelCoef;
		//ForceZ *= OpenAccelCoef;
	}
	else
	{
		//nlinfo("%f %f %f (%f %f)", ForceX, ForceY, ForceZ, Accel, CloseAccelCoef);
		float currentAccel = CloseAccelCoef * Accel;
		Force *= currentAccel;
		//nlinfo("clientForce ( %f %f %f )", clientForce.x, clientForce.y, clientForce.z);
		//nlinfo("Force = %f %f %f (%f * %f)", Force.x, Force.y, Force.z, CloseAccelCoef,Accel);
	}
	Accel = DefaultAccel;
}

void CEntity::position(NLMISC::CVector pos)
{
	Pos = pos;
	pausePhysics();
	dBodySetPosition(Body, Pos.x, Pos.y, Pos.z);
	dGeomSetPosition(Geom, Pos.x, Pos.y, Pos.z);
	dBodySetLinearVel(Body, 0.0f, 0.0f, 0.0f);
	Force = CVector::Null;
	resumePhysics();
}


void CEntity::maxLinearVelocity(float maxLinVel)
{
	MaxLinearVelocity = maxLinVel;
}

float CEntity::maxLinearVelocity()
{
	return MaxLinearVelocity;
}



float CEntity::meanVelocity()
{
	float res = 0;
	if(InGame)
	{
		const dReal *vel = dBodyGetLinearVel(Body);
		LastVel[LastVelPos%10]  =(float)fabs(vel[0]);
		LastVel[LastVelPos%10] +=(float)fabs(vel[1]);
		LastVel[LastVelPos%10] +=(float)fabs(vel[2]);
		LastVelPos++;
		
		for(uint i = 0; i < 10; i++)
			res += LastVel[i];
	}
	return res;
}	

float CEntity::defaultAccel()
{
	return DefaultAccel;
}

void CEntity::defaultAccel(float a)
{
	DefaultAccel = a;
}

void CEntity::enableOpenCloseCommand(bool e)
{
	EnableOpenCloseCommand = e;
}

void CEntity::enableCrashInFly(bool e)
{
	EnableCrashInFly = e;
}

string CEntity::team()
{
	string res = "";
	if(Name.size()<3)
		return res;
	string tmp = Name.substr(1);
	uint i = 0;
	for(i=0;i<tmp.size() && tmp[i]!=']';i++)
		res += tmp[i];
	if(tmp[i]!=']')
		res = "";
	return res;
}

void CEntity::voteMap(const string &voteMap)
{
	VoteMap = voteMap;
}

string CEntity::voteMap()
{
	return VoteMap;
}


bool CEntity::canSpeak()
{
	return CanSpeak	;
}

void CEntity::canSpeak(bool speak)
{
	CanSpeak = speak;
}


//
// Commands
//


//
//
//



