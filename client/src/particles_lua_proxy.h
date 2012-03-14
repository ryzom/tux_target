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

#ifndef _PARTICLES_LUA_PROXY_H_
#define _PARTICLES_LUA_PROXY_H_

#include "particles.h"
#include "../../common/lunar.h"

class CParticles;

class CParticlesProxy
{
	
public:
	CParticlesProxy(lua_State *luaSession)
	{
//		nlinfo("new CParticlesProxy0 0x%p",this);
		_particles = 0;
		LuaUserData = 0;
		LuaUserDataRef = 0;
	}
	
	CParticlesProxy(lua_State *luaSession, CParticles *particles)
	{
//		nlinfo("new CParticlesProxy1 0x%p(0x%p)",this,module);
		_particles = particles;
		LuaUserData = 0;
		LuaUserDataRef = 0;
	}
	
	~CParticlesProxy()
	{
//		nlinfo("delete CParticlesProxy 0x%p",this);
	}
	
	bool call(std::string funcName);

	int setMetatable(lua_State *L);
	int getUserData(lua_State *luaSession);
	int setUserData(lua_State *luaSession);
	
	int getName(lua_State *luaSession);
	int getPos(lua_State *luaSession);
	int setPos(lua_State *luaSession);
	int show(lua_State *luaSession);
	int hide(lua_State *luaSession);
	int start(lua_State *luaSession);
	int stop(lua_State *luaSession);
	
	
	static const char className[];	
	static Lunar<CParticlesProxy>::RegType methods[];	
private:
	void		*LuaUserData;
	int			 LuaUserDataRef;	
	CParticles *_particles;
	CLuaVector Pos;
};


#endif
