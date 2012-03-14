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

#include <string>

#include <nel/misc/path.h>

#include "luna.h"
#include "lua_nel.h"
#include "lua_utility.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Classes
//

static int lua_nlinfo(lua_State *L)
{
#if 0
	/* get the number of arguments (n) and
    check that the stack contains at least 1*/
	int n = lua_gettop(L);
	string msg = "";
	for(int i=0;i<n;i++)
	{
		msg += lua_tostring(L,i+1);
		msg += " ";
	}
	lua_pop(L,n);
	
	//const char *msg = lua_tostring(L,1);
	nlinfo("LUA : %s",msg.c_str());
#else
	const char *msg = lua_tostring(L,1);
	nlwarning("LUA : %s",msg);
	lua_pop(L,1);
#endif
	return 0;
}

static int lua_nlwarning(lua_State *L)
{
	const char *msg = lua_tostring(L,1);
	nlwarning("LUA : %s",msg);
	lua_pop(L,1);
	return 0;
}

static int lua_ALERT(lua_State *L)
{
	const char *msg = lua_tostring(L,1);
	char ch[1024];
	size_t i=0;
	while(msg[i]!='\0' && i<strlen(msg))
	{
		size_t j=0;
		while(msg[i]!='\0' && msg[i]!='\n' && i<strlen(msg))
			ch[j++]=msg[i++];
		if(msg[i]=='\n')
			i++;
		ch[j++]='\0';
		nlwarning("LUA_ALERT: %s",ch);
	}
	lua_pop(L,1);
	return 0;
}

static int lua_ERRORMESSAGE(lua_State *L)
{
	nlwarning("lua ERROR");
	return 0;
}

static int lua_exit(lua_State *L)
{
	const int exitCode = (int) lua_tonumber(L,-1);
	lua_pop(L,1);
	nlinfo("LUA : exit(%d)",exitCode);
	return 0;
}

static int mydefault_panic (lua_State *L) {
	nlwarning("lua panic");
	lua_close(L);
	return 0;
}

static int myTB = 0;

int luaGetTB()
{
	return myTB;
}

lua_State *luaOpen()
{
	lua_State *L = lua_open();
	if(!L)
	{
		nlwarning("LUA: lua_open() failed while trying to create ");
		return 0;
	}
	nlinfo("Lua open : L = 0x%p",L);
	
	lua_baselibopen(L);
	lua_iolibopen(L);
	lua_strlibopen(L);
	lua_mathlibopen(L);
	lua_dblibopen(L);
	lua_tablibopen(L);
	
	lua_register(L, "print", lua_nlinfo);
	lua_register(L, "warn", lua_nlwarning);
	lua_register(L, "error", lua_nlwarning);
	lua_register(L, "_ALERT", lua_ALERT);
	lua_register(L, "_TRACEBACK ", lua_ERRORMESSAGE);
	lua_register(L, "_ERRORMESSAGE ", lua_ERRORMESSAGE);
	lua_register(L, "exit", lua_exit);
	lua_atpanic(L,mydefault_panic);

	lua_settop(L, 0);
	lua_pushliteral(L, "_TRACEBACK");
	lua_gettable(L, LUA_GLOBALSINDEX);   // get traceback function
	myTB = lua_gettop(L);
	

	Lunar<CLuaVector>::Register(L);
	Lunar<CLuaAngleAxis>::Register(L);
	Lunar<CLuaRGBA>::Register(L);
	
	return L;
}

lua_State *luaOpenAndLoad(const string &filename)
{
	lua_State *L = luaOpen();
	bool res = luaLoad(L,filename);
	if(!res)
		L = 0;
	return L;
}

bool luaLoad(lua_State * L, const string &filename)
{
	if(!L)
	{
		nlwarning("LUA: lua_dofile() failed while trying to load '%s'", filename.c_str());
		return false;
	}

	string fn = CPath::lookup(filename, false);
	if(fn.empty())
	{
		nlwarning("LUA: File '%s' is not found", filename.c_str());
		return false;
	}
	
	int res = lua_dofile(L, fn.c_str());
	if(res > 0)
	{
		nlwarning("LUA: lua_dofile(\"%s\") failed with code %d", filename.c_str(), res);
		luaClose(L);
		return false;
	}
	return true;
}

void luaClose(lua_State *&L)
{
	if(!L)
	{
		nlwarning("LUA: Can't delete the L, already 0");
		return;
	}
	lua_setgcthreshold(L, 0);  // collected garbage
	lua_close(L);
	L = 0;
}

void luaGetVariable(lua_State *L, std::string &var)
{
	/*
	size_t chlen;
	const char *res = luaL_checklstring(L,-1,&chlen);
	var = res;
	*/
	
	int isString = lua_isstring(L,-1);
	if(isString)
		var = lua_tostring(L, -1);
	else
		nlwarning("luaGetVariable(string) not a string");
	//lua_pop(L, 1);
	
}
