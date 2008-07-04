// This file is part of Mtp Target.
// Copyright (C) 2008 Vialek
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// 
// Vianney Lecroart - gpl@vialek.com

#ifndef MT_LUA_NEL_H
#define MT_LUA_NEL_H


//
// Includes
//

#include <nel/misc/vector.h>
#include <nel/misc/rgba.h>

#include "lunar.h"


//
// Functions
//

void luaRegisterNeL(lua_State *L);


//
// Classes
//

class CLuaVector : public NLMISC::CVector
{
public:
	CLuaVector() : NLMISC::CVector(NLMISC::CVector::Null) { }
	CLuaVector(const NLMISC::CVector &v) : NLMISC::CVector(v) { }
	CLuaVector(float _x, float _y, float _z) : NLMISC::CVector(_x, _y, _z) { }

	CLuaVector(lua_State *L)
	{
		x = float(luaL_checknumber(L, 1));
		y = float(luaL_checknumber(L, 2));
		z = float(luaL_checknumber(L, 3));
	}

	int getX(lua_State *luaSession);
	int getY(lua_State *luaSession);
	int getZ(lua_State *luaSession);
	int setX(lua_State *luaSession);
	int setY(lua_State *luaSession);
	int setZ(lua_State *luaSession);

	static const char *className() { return "CVector"; }
	static Lunar<CLuaVector>::RegType *methods()
	{
		static Lunar<CLuaVector>::RegType m[] =
		{
			bind_method(CLuaVector, getX), bind_method(CLuaVector, setX),
			bind_method(CLuaVector, getY), bind_method(CLuaVector, setY),
			bind_method(CLuaVector, getZ), bind_method(CLuaVector, setZ),
			{ 0, 0 }
		};
		return m;
	}
};

class CLuaAngleAxis : public NLMISC::CAngleAxis
{
public:

	CLuaAngleAxis() : NLMISC::CAngleAxis(NLMISC::CVector::I, 0.0f) { }
	CLuaAngleAxis(const NLMISC::CAngleAxis &a) : NLMISC::CAngleAxis(a) { }
	CLuaAngleAxis(const NLMISC::CVector &_axis, float _angle) : NLMISC::CAngleAxis(_axis, _angle) { }

	CLuaAngleAxis(lua_State *L)
	{
		Axis.x = float(luaL_checknumber(L, 1));
		Axis.y = float(luaL_checknumber(L, 2));
		Axis.z = float(luaL_checknumber(L, 3));
		Angle = float(luaL_checknumber(L, 4));
	}

	static const char *className() { return "CAngleAxis"; }
	static Lunar<CLuaAngleAxis>::RegType *methods()
	{
		static Lunar<CLuaAngleAxis>::RegType m[] =
		{
			{ 0, 0 }
		};
		return m;
	}
};

class CLuaRGBA : public NLMISC::CRGBA
{
public:
	CLuaRGBA() : NLMISC::CRGBA(NLMISC::CRGBA::White) { }
	CLuaRGBA(const NLMISC::CRGBA &c) : NLMISC::CRGBA(c) { }
	CLuaRGBA(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255) : NLMISC::CRGBA(_r, _g, _b, _a) { }

	CLuaRGBA(lua_State *L)
	{
		R = uint8(luaL_checknumber(L, 1));
		G = uint8(luaL_checknumber(L, 2));
		B = uint8(luaL_checknumber(L, 3));
		A = uint8(luaL_optnumber(L, 4, lua_Number(255)));
	}

	static const char *className() { return "CRGBA"; }
	static Lunar<CLuaRGBA>::RegType *methods()
	{
		static Lunar<CLuaRGBA>::RegType m[] =
		{
			{ 0, 0 }
		};
		return m;
	}
};

#endif
