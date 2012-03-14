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

#ifndef LUA_NEL
#define LUA_NEL


//
// Includes
//

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

#include <nel/misc/vector.h>
#include <nel/misc/rgba.h>

#include "luna.h"
#include "lunar.h"

//
// Classes
//

class CLuaVector : public NLMISC::CVector
{
public:
	CLuaVector(NLMISC::CVector v):CVector(v) { }
	CLuaVector() { }
	CLuaVector(float	_x, float _y, float _z):CVector(_x,_y,_z) { }
	
	CLuaVector(lua_State *L)
	{
		x = (float)luaL_checknumber(L, 1);
		y = (float)luaL_checknumber(L, 2);
		z = (float)luaL_checknumber(L, 3);
	}
	
	static const char className[];
	static Lunar<CLuaVector>::RegType methods[];
};


class CLuaAngleAxis : public NLMISC::CAngleAxis
{
public:
	CLuaAngleAxis() { }
	CLuaAngleAxis(NLMISC::CVector _axis,float _angle):CAngleAxis(_axis,_angle) { }
	
	CLuaAngleAxis(lua_State *L)
	{
		Axis.x = (float)luaL_checknumber(L, 1);
		Axis.y = (float)luaL_checknumber(L, 2);
		Axis.z = (float)luaL_checknumber(L, 3);
		Angle = (float)luaL_checknumber(L, 4);
	}
	
	static const char className[];
	static Lunar<CLuaAngleAxis>::RegType methods[];
};

class CLuaRGBA : public NLMISC::CRGBA
{
public:
	CLuaRGBA() { }
	CLuaRGBA(uint8 _r,uint8 _g,uint8 _b,uint8 _a):CRGBA(_r,_g,_b,_a) { }
	
	CLuaRGBA(lua_State *L)
	{
		R = (uint8)luaL_checknumber(L, 1);
		G = (uint8)luaL_checknumber(L, 2);
		B = (uint8)luaL_checknumber(L, 3);
		A = (uint8)luaL_checknumber(L, 4);
	}
	
	static const char className[];
	static Lunar<CLuaRGBA>::RegType methods[];
};



#endif
