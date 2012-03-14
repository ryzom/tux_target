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
#include "lua_engine.h"
#include "network.h"


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
		bind_method(CModuleProxy, setCollide),	
		bind_method(CModuleProxy, setBounce),	
		bind_method(CModuleProxy, setScore),	
		bind_method(CModuleProxy, getScore),	
		bind_method(CModuleProxy, setAccel),	
		bind_method(CModuleProxy, setFriction),	
		bind_method(CModuleProxy, setEnabled),	
		bind_method(CModuleProxy, setVisible),	
		bind_method(CModuleProxy, getName),	
		bind_method(CModuleProxy, getId),	
		bind_method(CModuleProxy, setShapeName),	
		bind_method(CModuleProxy, getPos),	
		bind_method(CModuleProxy, setPos),	
		bind_method(CModuleProxy, setBounceCoef),	
		bind_method(CModuleProxy, setBounceVel),	
	{0,0}
};


bool CModuleProxy::call(string funcName)
{ 
//	nlinfo("CModuleProxy::call(%s)",funcName.c_str());
	int res;
	lua_State *L = CLuaEngine::getInstance().session();
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

int CModuleProxy::setBounce(lua_State *L)
{ 
	lua_Number b = luaL_checknumber(L,1);
	_module->bounce(b==1); 
	return 0; 
}

int CModuleProxy::setCollide(lua_State *L)
{ 
	lua_Number c = luaL_checknumber(L,1);
	_module->collide(c==1); 
	return 0; 
}

int CModuleProxy::setScore(lua_State *L)
{ 
	lua_Number score = luaL_checknumber(L,1);
	_module->score((uint32)score); 
	return 0; 
}

int CModuleProxy::getScore(lua_State *L)
{ 
	lua_Number score = _module->score();
	lua_pushnumber(L,score); 
	return 1; 
}

int CModuleProxy::setAccel(lua_State *L)
{ 
	lua_Number accel = luaL_checknumber(L,1);
	_module->accel((float)accel); 
	return 0; 
}

int CModuleProxy::setFriction(lua_State *L)
{ 
	lua_Number friction = luaL_checknumber(L,1);
	_module->friction((float)friction); 
	return 0; 
}

int CModuleProxy::setEnabled(lua_State *L)
{ 
	lua_Number enabled = luaL_checknumber(L,1);
	bool e = enabled==1;
	_module->enabled(e); 
	return 0; 
}

int CModuleProxy::setVisible(lua_State *L)
{ 
	lua_Number visible = luaL_checknumber(L,1);
	bool e = visible==1;
	_module->visible(e); 
	return 0; 
}

int CModuleProxy::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession, _module->name().c_str());
	return 1;
}

int CModuleProxy::getId(lua_State *luaSession)
{
	lua_Number id = _module->id();
	lua_pushnumber(luaSession,id); 
	return 1;
}

int CModuleProxy::setShapeName(lua_State *luaSession)
{
	size_t name_len;
	const char *name = luaL_checklstring(luaSession, 1, &name_len);
	_module->shapeName(name);
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

int CModuleProxy::setPos(lua_State *luaSession)
{
	CLuaVector pos  = *Lunar<CLuaVector>::check(luaSession,-1);
	//_module->position(pos);
	_module->changePosition(pos);
	return 0;
}

int CModuleProxy::getPos(lua_State *luaSession)
{
	Pos = _module->position();
	Lunar<CLuaVector>::push(luaSession,&Pos);
	return 1;
}


int CModuleProxy::setBounceCoef(lua_State *L)
{
	float c = (float)luaL_checknumber(L,1);
	_module->bounceCoef(c);
	return 0;	
}

int CModuleProxy::setBounceVel(lua_State *L)
{
	float v = (float)luaL_checknumber(L,1);
	_module->bounceVel(v);
	return 0;	
}
