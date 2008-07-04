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

#include <stack>

#include <nel/misc/path.h>

#include "lua_nel.h"
#include "lua_utility.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Classes
//

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
		nlerror("LUA_ALERT: %s",ch);
	}
	lua_pop(L,1);
	return 0;
}

static int lua_ERRORMESSAGE(lua_State *L)
{
	nlerror("lua ERROR");
	return 0;
}

static int lua_exit(lua_State *L)
{
	const int exitCode = (int) lua_tonumber(L,-1);
	lua_pop(L,1);
	nlerror("LUA : exit(%d)",exitCode);
	return 0;
}

static int lua_include(lua_State *L)
{
	const char *msg = lua_tostring(L,1);
	lua_pop(L,1);
	CLua::instance().execute(msg);
	return 0;
}

static int mydefault_panic (lua_State *L) {
	nlerror("LUA PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
	lua_close(L);
	return 0;
}

void CLua::init()
{
	release();

	L = lua_open();
	nlassert(L);

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
	luaL_openlibs(L);
#else
	luaopen_base(L);
	luaopen_table(L);
	luaopen_io(L);
	luaopen_string(L);
	luaopen_math(L);
	luaopen_debug(L);
#endif

	lua_register(L, "_ALERT", lua_ALERT);
	lua_register(L, "_TRACEBACK ", lua_ERRORMESSAGE);
	lua_register(L, "_ERRORMESSAGE ", lua_ERRORMESSAGE);
	lua_register(L, "exit", lua_exit);
	lua_register(L, "include", lua_include);
	lua_atpanic(L, mydefault_panic);

	lua_settop(L, 0);
	lua_pushliteral(L, "_TRACEBACK");
	lua_gettable(L, LUA_GLOBALSINDEX);   // get traceback function

	luaRegisterNeL(L);
}

void CLua::execute(const string &filename)
{
	nlassert(L);

	string fn = CPath::lookup(filename);
	int res = luaL_loadfile(L,fn.c_str());
	if (res)
	{
		string err (lua_tostring(L, -1));
		if (err.empty()) err = "(error with no message)";
		nlerror("loadfile error(%s) : (status = %d) %s", filename.c_str(), res, err.c_str());
		return;
	}
	res = lua_pcall(L,0,0,0);
	if (res)
	{
		string err (lua_tostring(L, -1));
		if (err.empty()) err = "(error with no message)";
		nlerror("pcall error(%s) : (status = %d) %s", filename.c_str(), res, err.c_str());
		return;
	}
}

void CLua::call(const string &func)
{
	lua_getglobal(L, func.c_str());
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		nlstop;
		return;
	}
	int res = lua_pcall (L,0,0,0);
	if (res)
	{
		string err (lua_tostring(L, -1));
		if (err.empty()) err = "(error with no message)";
		nlerror("pcall error(%s) : (status = %d) %s", func.c_str(), res, err.c_str());
		return;
	}
}

void CLua::release()
{
	if(L)
	{
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
	    lua_gc(L, LUA_GCCOLLECT, 0);
#else
		lua_setgcthreshold(L, 0);  // collected garbage
#endif
		lua_close(L);
		L = 0;
	}
}

bool CLua::globalVariableExists( const string &varname )
{
	lua_getglobal(L, varname.c_str());
	bool nil = lua_isnil(L, -1);
	lua_pop(L, 1);
	return !nil;
}

void luaGetVariable(lua_State *L, string &var)
{
	int isString = lua_isstring(L,-1);
	if(isString)
	{
		var = lua_tostring(L, -1);
	}
	else
	{
		nlwarning("luaGetVariable(string) not a string");
	}
	lua_pop(L, 1);
}


string CLua::displayStack(bool display)
{
	H_AUTO2;
	string str(NLMISC::toString("lua stack size %d: ", lua_gettop(L)));
	for(int i = 0; i < lua_gettop(L); i++) { str += lua_typename(L, lua_type(L, -i-1)); str += " "; }
	if(display) nlinfo(str.c_str());
	return str;
}

struct checkent
{
	string beforeStack;
	lua_Number value;
};

static stack<checkent> Checks;

void CLua::checkBefore()
{
	H_AUTO2;

	checkent ce;
	ce.beforeStack = displayStack(false);
	ce.value = rand();
	lua_pushnumber(L, ce.value);
	Checks.push(ce);
}

void CLua::checkAfter()
{
	H_AUTO2;

	checkent ce = Checks.top();
	Checks.pop();
	lua_Number n = lua_tonumber(L, -1);
	lua_Number n2 = ce.value;
	if(n != n2)
	{
		nlinfo("lua check failed: should be %f and is %f (check size is %d)", n2, n, Checks.size());
		nlinfo("before check: %s", ce.beforeStack.c_str());
		displayStack();
		nlstopex(("LUA CHECK failed stack size changed"));
	}
	lua_pop(L, 1);
}

static void _callalert (lua_State *L, int status) {
	if (status != 0) {
		lua_getglobal(L, "_ALERT");
		if (lua_isfunction(L, -1)) {
			lua_insert(L, -2);
			lua_call(L, 1, 0);
		}
		else {  /* no _ALERT function; print it on stderr */
			fprintf(stderr, "%s\n", lua_tostring(L, -2));
			lua_pop(L, 2);  /* remove error message and _ALERT */
		}
	}
}

static int _aux_do (lua_State *L, int status) {
	if (status == 0) {  /* parse OK? */
		status = lua_pcall(L, 0, LUA_MULTRET, 0);  /* call main */
	}
	_callalert(L, status);
	return status;
}

int luaDoBuffer (lua_State *L, const char *buff, size_t size, const char *name)
{
	return _aux_do(L, luaL_loadbuffer(L, buff, size, name));
}

int luaDoString (lua_State *L, const char *str)
{
	return luaDoBuffer (L, str, strlen(str), str);
}
