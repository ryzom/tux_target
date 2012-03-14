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

#ifndef LUA_UTILITY_H
#define LUA_UTILITY_H


//
// Includes
//

#include <vector>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "luna.h"
#include "lunar.h"


//
// Classes
//

// Create a new lua state and loda the lua script 'filename'
lua_State *luaOpenAndLoad(const std::string &filename);

lua_State *luaOpen();

bool luaLoad(lua_State * L, const std::string &filename);

// Clean and close a lua state
void luaClose(lua_State *&L);

// Initialize a C variable with a global lua variable
//
// for example:
//
//  string LevelName;
//  luaGetGlobalVariable(L,LevelName);
//

#define luaGetGlobalVariable(_l,_varname)                   \
lua_getglobal(_l, #_varname);                               \
if (lua_isnil(_l, -1))                                      \
{                                                           \
	nlwarning("luaGetGlobal : %s not found",#_varname);     \
	lua_pop(_l, 1);                                         \
}                                                           \
else                                                        \
	luaGetVariable(_l, _varname);

#define luaGetGlobalVector(_l,_varname)                     \
lua_getglobal(_l, #_varname);                               \
if (lua_isnil(_l, -1))                                      \
{                                                           \
	nlwarning("luaGetGlobal : %s not found",#_varname);     \
	lua_pop(_l, 1);                                         \
}                                                           \
else                                                        \
	luaGetVector(_l, _varname);

#define luaGetGlobalVectorWithName(_l,_var,_varname)        \
lua_getglobal(_l, _varname);                                \
if (lua_isnil(_l, -1))                                      \
{                                                           \
	nlwarning("luaGetGlobal : %s not found",#_varname);     \
	lua_pop(_l, 1);                                         \
}                                                           \
else                                                        \
	luaGetVector(_l, _var);


// Initialize a C variable with the lua variable on top of the stack

inline void luaGetVariable(lua_State *L, bool &var)
{
	var = lua_toboolean(L, -1)!=0;
	lua_pop(L, 1);
}

// this macro is used to set luaGetVariable for all number types
#define IMP_GET_VAR_INT(_a) \
inline void luaGetVariable(lua_State *L, _a &var) \
{ \
	var = (_a)lua_tonumber(L, -1); \
	lua_pop(L, 1); \
}

IMP_GET_VAR_INT(double);
IMP_GET_VAR_INT(float);
IMP_GET_VAR_INT(uint32);
IMP_GET_VAR_INT(uint16);
IMP_GET_VAR_INT(uint8);
IMP_GET_VAR_INT(sint32);
IMP_GET_VAR_INT(sint16);
IMP_GET_VAR_INT(sint8);

void luaGetVariable(lua_State *L, std::string &var);

template<class T>
void luaGetVector(lua_State *L, std::vector<T> &var)
{
	lua_pushnil(L);
	while(lua_next(L, -2) != 0)
	{
		// `key' is at index -2 and `value' at index -1
		T v;
		luaGetVariable(L, v);
		var.push_back(v);
		lua_pop(L, 1);  // removes `value'; keeps `key' for next iteration
	}
	lua_pop(L, 1);  // removes `key'
}

template<class T>
void luaGetVariable(lua_State *L, T &var)
{
	var = *Lunar<T>::check(L, -1);
}

#define CHECK_BEFORE(L) lua_pushnumber(L, 1234.5678)
#define CHECK_AFTER(L) { nlassert(lua_tonumber(L, -1) == 1234.5678); lua_pop(L, 1); }

int luaGetTB();



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// OLD SKEET CODE
#if 0

template <class T> class CLuaProxy
{
public:
	// member function map
	struct RegType
	{
		const char *name;
		const int(T::*mfunc)(lua_State *);
	};

	// register class T
	static void reg(lua_State *luaSession)
	{
		nlassert(ObjectTag == 0);
		//lua_register(luaSession, ClassName, &luaProxy<T>::Constructor);
//		ObjectTag = lua_newtag(luaSession);
//		lua_pushcfunction(luaSession, &luaProxy<T>::GarbageCollector);
//		lua_settagmethod(luaSession, ObjectTag, "gc"); // tm to release objects
	}

	static void unreg(lua_State *luaSession)
	{
		ObjectTag = 0;
	}
	
	static int initAndPush(lua_State *luaSession,T *obj)
	{
		lua_newtable(luaSession); // new table object
		lua_pushnumber(luaSession, 0); // userdata obj at index 0

		lua_newuserdatabox(luaSession,obj);
		lua_settag(luaSession,ObjectTag);
		
		//lua_pushusertag(luaSession, obj, ObjectTag); // have gc call tm

		lua_settable(luaSession, -3);
		
		// register the member functions
		for(int i = 0; FunctionList[i].name; i++)
		{
			lua_pushstring(luaSession, FunctionList[i].name);
			lua_pushnumber(luaSession, i);
			// TODO : look with this function is not found on linux lua5 package 
			//lua_pushcclosure(luaSession, &luaProxy<T>::Thunk, 1);
			lua_settable(luaSession, -3);
			}
		return 1; // return the table object
	}

	static int ObjectTag; // object tag
private:

	// member function dispatcher
	static int thunk(lua_State *luaSession)
	{
		// stack = closure(-1), [args...], 'self' table(1)
		int i = static_cast<int>(lua_tonumber(luaSession, -1));
		lua_pushnumber(luaSession, 0); // userdata object at index 0
		lua_gettable(luaSession, 1);
		T *obj = static_cast<T *>(lua_touserdata(luaSession, -1));
		lua_pop(luaSession, 2); // pop closure value and obj
		return (obj->*(FunctionList[i].mfunc))(luaSession);
	}

	// constructs T objects
	static int Constructor(lua_State *luaSession)
	{
		T *obj= new T(luaSession);
		return InitAndPush(luaSession,obj);
	}

	// releases objects
	static int garbageCollector(lua_State *luaSession)
	{
		return 0;
	}
	
protected:

	CLuaProxy(); // hide default constructor

	static const char *ClassName;
	static const RegType FunctionList[];
};

template <class T> int CLuaProxy<T>::ObjectTag = 0;
#endif


#endif // LUA_UTILITY_H
