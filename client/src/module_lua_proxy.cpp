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
#include "module_lua_proxy.h"
#include "level_manager.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
//
//

const char CModuleProxy::className[] = "Module";

Lunar<CModuleProxy>::RegType CModuleProxy::methods[] = 
{
	bind_method(CModuleProxy, setMetatable),	
		bind_method(CModuleProxy, getUserData),	
		bind_method(CModuleProxy, setUserData),	
		bind_method(CModuleProxy, getName),	
		bind_method(CModuleProxy, setColor),	
		bind_method(CModuleProxy, setPos),	
		bind_method(CModuleProxy, getPos),	
	{0,0}
};


bool CModuleProxy::call(string funcName)
{ 
//	nlinfo("CModuleProxy::call(%s)",funcName.c_str());
	int res;
	if(!CLevelManager::getInstance().levelPresent())
		return false;
	lua_State *L = CLevelManager::getInstance().currentLevel().luaState();
	if(L==0)
		return false;
	int mp = Lunar<CModuleProxy>::push(L, this);
	res = Lunar<CModuleProxy>::call(L,funcName.c_str(),0);
	/*
	if(res<0)
		nlwarning("error calling lua function : %s",funcName.c_str());
	*/

	lua_pop(L,-1);
	return res>=0;
}


int CModuleProxy::setMetatable(lua_State *L)
{ 
	int res = lua_setmetatable(L, -2);
	if(res==0)
		nlwarning("cannot set metatable");
	return 0; 
}


int CModuleProxy::getUserData(lua_State *luaSession)
{
	lua_getref(luaSession, LuaUserDataRef); //push obj which have this ref id
	/*
	if(!LuaUserDataRef)
		nlwarning("clien lua call getuserdata but userdata is 0");
	*/
	return 1; // one return value
}

int CModuleProxy::setUserData(lua_State *luaSession)
{
	LuaUserData = lua_touserdata(luaSession, 1); // get arg
	LuaUserDataRef = lua_ref(luaSession,1); //get ref id and lock it
	return 0; // no return value
}

int CModuleProxy::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession, _module->name().c_str());
	return 1;
}

int CModuleProxy::setColor(lua_State *luaSession)
{
	uint8 r = (uint8 )luaL_checknumber(luaSession,1);
	uint8 g = (uint8 )luaL_checknumber(luaSession,2);
	uint8 b = (uint8 )luaL_checknumber(luaSession,3);
	uint8 a = (uint8 )luaL_checknumber(luaSession,4);
	_module->color(CRGBA(r,g,b,a));
	return 0;	
}


int CModuleProxy::getPos(lua_State *luaSession)
{
	Pos = _module->position();
	Lunar<CLuaVector>::push(luaSession,&Pos);
	return 1;
}

int CModuleProxy::setPos(lua_State *luaSession)
{
	CLuaVector pos  = *Lunar<CLuaVector>::check(luaSession,-1);
	_module->position(pos);
	return 0;
}

