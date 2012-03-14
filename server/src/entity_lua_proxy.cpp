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
#include "network.h"
#include "lua_engine.h"
#include "level_manager.h"
#include "session_manager.h"
#include "entity_lua_proxy.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
//
//

const char CEntityProxy::className[] = "Entity";

Lunar<CEntityProxy>::RegType CEntityProxy::methods[] = 
{
	bind_method(CEntityProxy, setMetatable),	
		bind_method(CEntityProxy, getUserData),	
		bind_method(CEntityProxy, setUserData),	
		bind_method(CEntityProxy, getName),	
		bind_method(CEntityProxy, enableOpenCloseCommand),	
		bind_method(CEntityProxy, enableCrashInFly),	
		bind_method(CEntityProxy, getOpenCloseCount),	
		bind_method(CEntityProxy, setOpenCloseCount),	
		bind_method(CEntityProxy, getOpenCloseMax),	
		bind_method(CEntityProxy, setOpenCloseMax),	
		bind_method(CEntityProxy, getMaxLinearVelocity),	
		bind_method(CEntityProxy, setMaxLinearVelocity),	
		bind_method(CEntityProxy, getEid),	
		bind_method(CEntityProxy, getPos),	
		bind_method(CEntityProxy, setPos),	
		bind_method(CEntityProxy, getStartPointPos),	
		bind_method(CEntityProxy, getStartPointId),	
		bind_method(CEntityProxy, setStartPointId),	
		bind_method(CEntityProxy, getIsSpectator),	
		bind_method(CEntityProxy, getIsOpen),	
		bind_method(CEntityProxy, setIsOpen),	
		bind_method(CEntityProxy, setCurrentScore),	
		bind_method(CEntityProxy, getCurrentScore),	
		bind_method(CEntityProxy, displayText),	
		bind_method(CEntityProxy, setDensity),	
		bind_method(CEntityProxy, setDefaultAccel),	
		bind_method(CEntityProxy, getDefaultAccel),	
		bind_method(CEntityProxy, setDefaultFriction),	
		bind_method(CEntityProxy, getDefaultFriction),	
		bind_method(CEntityProxy, setFreezCommand),	
		bind_method(CEntityProxy, setArrivalTime),	
		bind_method(CEntityProxy, getArrivalTime),	
		bind_method(CEntityProxy, getTeam),	
		bind_method(CEntityProxy, getMeanVelocity),	
	{0,0}
};


bool CEntityProxy::call(string funcName)
{ 
	int res;
	lua_State *L = CLuaEngine::getInstance().session();
	if(L==0)
		return false;
	int mp = Lunar<CEntityProxy>::push(L, this);
	res = Lunar<CEntityProxy>::call(L,funcName.c_str(),0);
	/*
	if(res<0)
		nlwarning("error calling lua function : %s",funcName.c_str());
	*/
	lua_pop(L,-1);
	return res>=0;
}

int CEntityProxy::setMetatable(lua_State *L)
{ 
    int metatable = lua_gettop(L);
	int res = lua_setmetatable(L, -2);
	if(res==0)
		nlwarning("cannot set metatable");
	return 0; 
}

int CEntityProxy::getUserData(lua_State *luaSession)
{
	lua_getref(luaSession, LuaUserDataRef); //push obj which have this ref id
	/*
	if(!LuaUserDataRef)
		nlwarning("clien lua call getuserdata but userdata is 0");
	*/
	return 1; // one return value
}

int CEntityProxy::setUserData(lua_State *luaSession)
{
	LuaUserData = lua_touserdata(luaSession, 1); // get arg
	LuaUserDataRef = lua_ref(luaSession,1); //get ref id and lock it
	return 0; // no return value
}

int CEntityProxy::getOpenCloseMax(lua_State *luaSession)
{
	lua_Number moc = _entity->MaxOpenClose;
	lua_pushnumber(luaSession, moc); 
	return 1;
}

int CEntityProxy::setOpenCloseMax(lua_State *luaSession)
{
	uint32 moc = (uint32)luaL_checknumber(luaSession,1);
	_entity->MaxOpenClose = moc;
	return 0;
}

int CEntityProxy::getMaxLinearVelocity(lua_State *luaSession)
{
	lua_Number mlv = _entity->maxLinearVelocity();
	lua_pushnumber(luaSession, mlv); 
	return 1;
}

int CEntityProxy::setMaxLinearVelocity(lua_State *luaSession)
{
	float mlv = (float)luaL_checknumber(luaSession,1);
	_entity->maxLinearVelocity(mlv);
	return 0;
}

int CEntityProxy::getOpenCloseCount(lua_State *luaSession)
{
	lua_Number noc = _entity->NbOpenClose;
	lua_pushnumber(luaSession, noc); 
	return 1;
}

int CEntityProxy::setOpenCloseCount(lua_State *luaSession)
{
	uint32 noc = (uint32)luaL_checknumber(luaSession,1);
	
	bool oldOpenCloseState = (_entity->NbOpenClose%2)==1;
	bool newOpenCloseState = (noc%2)==1;
	if(oldOpenCloseState!=newOpenCloseState)
	{
		CNetMessage msgout(CNetMessage::OpenClose);
		uint8 eid = _entity->id();
		msgout.serial(eid);
		CNetwork::getInstance().send(msgout);
	}
	
	_entity->NbOpenClose = noc;
	return 0;
}

int CEntityProxy::setFreezCommand(lua_State *luaSession)
{
	bool freez = luaL_checknumber(luaSession,1)==1;
	_entity->FreezeCommand = freez;
	return 0;
}

int CEntityProxy::getEid(lua_State *luaSession)
{
	lua_Number eid = _entity->id();
	lua_pushnumber(luaSession, eid); 
	return 1;
}

int CEntityProxy::getPos(lua_State *luaSession)
{
	entityPos = _entity->position();
	Lunar<CLuaVector>::push(luaSession,&entityPos);
	return 1;
}

int CEntityProxy::setPos(lua_State *luaSession)
{
	CLuaVector pos  = *Lunar<CLuaVector>::check(luaSession,-1);
	_entity->position(pos);
	return 0;
}

int CEntityProxy::getStartPointPos(lua_State *luaSession)
{
	lua_Number id = _entity->StartingPointId;
	startPos = CLevelManager::getInstance().currentLevel().getStartPoint((uint32)id)->position();
	Lunar<CLuaVector>::push(luaSession,&startPos);
	return 1;
}

int CEntityProxy::getStartPointId(lua_State *luaSession)
{
	lua_Number id = _entity->StartingPointId;
	lua_pushnumber(luaSession, id); 
	return 1;
}

int CEntityProxy::setStartPointId(lua_State *luaSession)
{
	uint8 id = (uint8)luaL_checknumber(luaSession,1);
	_entity->startPointId(id);
	return 0; // no return value
}

int CEntityProxy::getIsSpectator(lua_State *luaSession)
{
	lua_Number isSpec = _entity->spectator() ? 1 : 0;
	lua_pushnumber(luaSession, isSpec); 
	return 1;	
}

int CEntityProxy::getIsOpen(lua_State *luaSession)
{
	lua_Number isOpen = _entity->isOpen() ? 1 : 0;
	lua_pushnumber(luaSession, isOpen); 
	return 1;
}

int CEntityProxy::setIsOpen(lua_State *luaSession)
{
	lua_Number isOpen = luaL_checknumber(luaSession,1);
	_entity->isOpen(isOpen==1);
	return 0; // no return value
}

int CEntityProxy::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession, _entity->name().c_str());
	return 1;
}

int CEntityProxy::setCurrentScore(lua_State *luaSession)
{
	lua_Number score = luaL_checknumber(luaSession,1);
	_entity->CurrentScore = ((uint32)score); 

	static int oldscore = 0;
	if(oldscore != _entity->CurrentScore)
	{
//		nlinfo(">> %s set score with lua = %d",_entity->name().c_str(),_entity->CurrentScore);
		oldscore = _entity->CurrentScore;
	}
	return 0;
}

int CEntityProxy::getCurrentScore(lua_State *luaSession)
{
	lua_Number score = _entity->CurrentScore;
	lua_pushnumber(luaSession, score); 
	return 1;
}


int CEntityProxy::displayText(lua_State *luaSession)
{
	float x = (float)luaL_checknumber(luaSession,1);
	float y = (float)luaL_checknumber(luaSession,2);
	float scale = (float)luaL_checknumber(luaSession,3);

	uint8 r = (uint8 )luaL_checknumber(luaSession,4);
	uint8 g = (uint8 )luaL_checknumber(luaSession,5);
	uint8 b = (uint8 )luaL_checknumber(luaSession,6);
	size_t len;
	const char *text = luaL_checklstring(luaSession, 7, &len);

	CRGBA col(r,g,b,255);
	double duration = luaL_checknumber(luaSession,8);

	if(_entity->type() == CEntity::Client)
	{
		string message = text;
		
		CNetMessage msgout(CNetMessage::DisplayText);
		msgout.serial(x);
		msgout.serial(y);
		msgout.serial(scale);
		msgout.serial(message);
		msgout.serial(col);
		msgout.serial(duration);
		CNetwork::getInstance().send(_entity->id(),msgout);
//too much flood		nlinfo("display message : %s to %s",message.c_str(),_entity->name().c_str());
	}

	return 0;
}

int CEntityProxy::setDensity(lua_State *luaSession)
{
	lua_Number density = luaL_checknumber(luaSession,1);
	dMass m;
	dReal radius = 0.01f;
	dMassSetSphere(&m, (dReal)density, radius);
	dBodySetMass(_entity->Body, &m);
	return 0;
}


int CEntityProxy::setDefaultAccel(lua_State *luaSession)
{
	lua_Number daccel = luaL_checknumber(luaSession,1);
	_entity->defaultAccel((float)daccel);
	return 0;
}

int CEntityProxy::getDefaultAccel(lua_State *luaSession)
{
	lua_Number daccel = _entity->defaultAccel();
	lua_pushnumber(luaSession, daccel); 
	return 1;
}


int CEntityProxy::setDefaultFriction(lua_State *luaSession)
{
	lua_Number dfric= luaL_checknumber(luaSession,1);
	_entity->DefaultFriction = (float)dfric;
	return 0;	
}

int CEntityProxy::getDefaultFriction(lua_State *luaSession)
{
	lua_Number dfric = _entity->DefaultFriction;
	lua_pushnumber(luaSession, dfric); 
	return 1;	
}

int CEntityProxy::getArrivalTime(lua_State *luaSession)
{
	lua_Number res  = 0;
	res = _entity->ArrivalTime;
	lua_pushnumber(luaSession, res); 
	return 1;	
}

int CEntityProxy::setArrivalTime(lua_State *luaSession)
{
	TTime currentTime = CTime::getLocalTime();
	lua_Number newVal = (currentTime - CSessionManager::getInstance().startTime())/1000.0f;
	newVal = luaL_optnumber(luaSession,1,newVal);
	_entity->ArrivalTime =(float)newVal;
	return 0;	
}

int CEntityProxy::enableOpenCloseCommand(lua_State *luaSession)
{
	bool e = luaL_checknumber(luaSession,1)==1;
	_entity->enableOpenCloseCommand(e);
	return 0;	
}

int CEntityProxy::enableCrashInFly(lua_State *luaSession)
{
	bool e = luaL_checknumber(luaSession,1)==1;
	_entity->enableCrashInFly(e);
	return 0;	
}

int CEntityProxy::getTeam(lua_State *luaSession)
{
	uint teamCount = (uint)luaL_checknumber(luaSession,1);
	lua_Number res = 0;
	uint teamId = CEntityManager::getInstance().getTeam(_entity->id(),teamCount);
	res = teamId;
	lua_pushnumber(luaSession, res); 
	return 1;	
}


int CEntityProxy::getMeanVelocity(lua_State *luaSession)
{
	lua_Number res = _entity->meanVelocity();
	lua_pushnumber(luaSession, res); 
	return 1;	
}


