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
#include "level_manager.h"
#include "entity_lua_proxy.h"
#include "resource_manager2.h"


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
		bind_method(CEntityProxy, setCurrentScore),	
		bind_method(CEntityProxy, setColor),	
		bind_method(CEntityProxy, cacheFile),	
		bind_method(CEntityProxy, setStartPointId),	
	{0,0}
};


bool CEntityProxy::call(string funcName)
{ 
	int res;
	if(!CLevelManager::getInstance().levelPresent())
		return false;
	lua_State *L = CLevelManager::getInstance().currentLevel().luaState();
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

int CEntityProxy::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession, _entity->name().c_str());
	return 1;
}

int CEntityProxy::setCurrentScore(lua_State *luaSession)
{
	lua_Number score = luaL_checknumber(luaSession,1);
	_entity->currentScore((sint32)score);
	return 0;	
}

int CEntityProxy::setColor(lua_State *luaSession)
{
	uint8 r = (uint8 )luaL_checknumber(luaSession,1);
	uint8 g = (uint8 )luaL_checknumber(luaSession,2);
	uint8 b = (uint8 )luaL_checknumber(luaSession,3);
	uint8 a = (uint8 )luaL_checknumber(luaSession,4);
	_entity->color(CRGBA(r,g,b,a));
	return 0;	
}

int CEntityProxy::cacheFile(lua_State *luaSession)
{
	size_t len;
	const char *text = luaL_checklstring(luaSession, 1, &len);
	string filename(text);
	CResourceManager::getInstance().get(filename);
	return 0;	
}

int CEntityProxy::setStartPointId(lua_State *luaSession)
{
	uint8 startPointId = (uint8)luaL_checknumber(luaSession,1);
	_entity->startPointId(startPointId);
	return 0;	
}


