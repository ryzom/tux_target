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


//
// Includes
//

#include "stdpch.h"

#include <nel/misc/path.h>

#include "lua_nel.h"


//
// Classes
//

int CLuaVector::getX(lua_State *luaSession)
{
	lua_Number X = this->x;
	lua_pushnumber(luaSession, X);
	return 1;
}

int CLuaVector::getY(lua_State *luaSession)
{
	lua_Number Y = this->y;
	lua_pushnumber(luaSession, Y);
	return 1;
}

int CLuaVector::getZ(lua_State *luaSession)
{
	lua_Number Z = this->z;
	lua_pushnumber(luaSession, Z);
	return 1;
}

int CLuaVector::setX(lua_State *luaSession)
{
	lua_Number X = luaL_checknumber(luaSession,1);
	this->x = (float)X;
	return 0;
}

int CLuaVector::setY(lua_State *luaSession)
{
	lua_Number Y = luaL_checknumber(luaSession,1);
	this->y = (float)Y;
	return 0;
}

int CLuaVector::setZ(lua_State *luaSession)
{
	lua_Number Z = luaL_checknumber(luaSession,1);
	this->z = (float)Z;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

static int lua_nlinfo(lua_State *L)
{
	const char *msg = lua_tostring(L,1);

	lua_Debug ar;
	if(lua_getstack(L, 1, &ar) == 1 && lua_getinfo(L, "lS", &ar) != 0)
	{
		NLMISC::InfoLog->setPosition(ar.currentline, ar.short_src, "");
		NLMISC::InfoLog->displayNL("LUA: %s", msg);
	}
	else
	{
		nlinfo("LUA: %s", msg);
	}

	lua_pop(L,1);
	return 0;
}

static int lua_nlwarning(lua_State *L)
{
	const char *msg = lua_tostring(L,1);

	lua_Debug ar;
	if(lua_getstack(L, 1, &ar) == 1 && lua_getinfo(L, "lS", &ar) != 0)
	{
		NLMISC::WarningLog->setPosition(ar.currentline, ar.short_src, "");
		NLMISC::WarningLog->displayNL("LUA: %s", msg);
	}
	else
	{
		nlwarning("LUA: %s", msg);
	}

	lua_pop(L,1);
	return 0;
}

static int lua_nlerror(lua_State *L)
{
	const char *msg = lua_tostring(L,1);

	lua_Debug ar;
	if(lua_getstack(L, 1, &ar) == 1 && lua_getinfo(L, "lS", &ar) != 0)
	{
		NLMISC::ErrorLog->setPosition(ar.currentline, ar.short_src, "");
		NLMISC::ErrorLog->displayNL("LUA: %s", msg);
	}
	else
	{
		nlerror("LUA: %s", msg);
	}

	lua_pop(L,1);
	return 0;
}

void luaRegisterNeL(lua_State *L)
{
	nlassert(L);

	lua_register(L, "nlinfo", lua_nlinfo);
	lua_register(L, "nlwarning", lua_nlwarning);
	lua_register(L, "nlerror", lua_nlerror);

	Lunar<CLuaVector>::registerMethods(L);
	Lunar<CLuaAngleAxis>::registerMethods(L);
	Lunar<CLuaRGBA>::registerMethods(L);
}
