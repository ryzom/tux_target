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

#ifndef _ENTITY_LUA_PROXY_H_
#define _ENTITY_LUA_PROXY_H_

#include "entity.h"
#include "../../common/lunar.h"

class CEntityProxy
{
	
public:
	CEntityProxy(lua_State *luaSession, CEntity *entity)
	{
		_entity = entity;
		LuaUserData = 0;
		LuaUserDataRef = 0;
	}
	
	CEntityProxy(lua_State *luaSession)
	{
		_entity = 0;
		LuaUserData = 0;
		LuaUserDataRef = 0;
	}
	
	~CEntityProxy()
	{
	}
	
	bool call(std::string funcName);
	bool exec(std::string code);
	
	int setMetatable(lua_State *L);
	int getUserData(lua_State *luaSession);
	int setUserData(lua_State *luaSession);
	int getName(lua_State *luaSession);
	int setCurrentScore(lua_State *luaSession);
	int setColor(lua_State *luaSession);
	int cacheFile(lua_State *luaSession);
	int setStartPointId(lua_State *luaSession);
	
	static const char className[];	
	static Lunar<CEntityProxy>::RegType methods[];	
private:
	void				*LuaUserData;
	uint32				 LuaUserDataRef;
	CEntity *_entity;
};


#endif
