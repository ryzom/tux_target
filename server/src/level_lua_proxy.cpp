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
#include "entity_lua_proxy.h"
#include "lua_engine.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
//
//

const char CLevelProxy::className[] = "Level";

Lunar<CLevelProxy>::RegType CLevelProxy::methods[] = 
{
	bind_method(CLevelProxy, setMetatable),	
		bind_method(CLevelProxy, getUserData),	
		bind_method(CLevelProxy, setUserData),	
		bind_method(CLevelProxy, getName),	
	{0,0}
};


bool CLevelProxy::call(string funcName)
{ 
	int res;
	lua_State *L = CLuaEngine::getInstance().session();
	if(L==0)
		return false;
	int mp = Lunar<CLevelProxy>::push(L, this);
	res = Lunar<CLevelProxy>::call(L,funcName.c_str(),0);
	/*
	if(res<0)
		nlwarning("error calling lua function : %s",funcName.c_str());
	*/
	lua_pop(L,-1);
	return res>=0;
}

int CLevelProxy::setMetatable(lua_State *L)
{ 
    int metatable = lua_gettop(L);
	int res = lua_setmetatable(L, -2);
	if(res==0)
		nlwarning("cannot set metatable");
	return 0; 
}

int CLevelProxy::getUserData(lua_State *luaSession)
{
	lua_getref(luaSession, LuaUserDataRef); //push obj which have this ref id
	/*
	if(!LuaUserDataRef)
		nlwarning("clien lua call getuserdata but userdata is 0");
	*/
	return 1; // one return value
}

int CLevelProxy::setUserData(lua_State *luaSession)
{
	LuaUserData = lua_touserdata(luaSession, 1); // get arg
	LuaUserDataRef = lua_ref(luaSession,1); //get ref id and lock it
	return 0; // no return value
}

int CLevelProxy::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession, _level->name().c_str());
	return 1;
}

