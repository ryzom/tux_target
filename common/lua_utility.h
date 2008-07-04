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

#ifndef MT_LUA_UTILITY_H
#define MT_LUA_UTILITY_H


//
// Includes
//

#include "lunar.h"


//
// Classes
//

class CLua : public NLMISC::CSingleton<CLua>
{
public:
	CLua() : L(0) { }

	// prepare a new lua VM and do inits
	void init();
	void update() { }
	void release();

	// load and execute a lua file
	void execute(const std::string &filename);

	// call the function from the session
	void call(const std::string &func);

	lua_State *l() { return L; }

	// return true if a global variable named varname exists
	bool globalVariableExists(const string &varname);

	// display stack
	string displayStack(bool display = true);

	// check the stack before & after to be sure the stack is in the same state before & after
	// WARNING: these functions are really slow (10ms for checkBefore in debug)
	void checkBefore();
	void checkAfter();

private:

	lua_State *L;
};

int luaDoBuffer (lua_State *L, const char *buff, size_t size, const char *name);
int luaDoString (lua_State *L, const char *str);


// Initialize a C variable with a global lua variable
//
// All luaGet* gets the value and remove it from the stack
//
// for example:
//
//  string LevelName;
//  luaGetGlobalVariable(L,LevelName);
//

#define luaGetGlobalVariable(_l,_varname)                   \
{															\
	CLua::instance().checkBefore();						\
	lua_getglobal(_l, #_varname);                           \
	if (lua_isnil(_l, -1))                                  \
	{                                                       \
		nlwarning("luaGetGlobal : %s not found",#_varname); \
		lua_pop(_l, 1);										\
	}                                                       \
	else													\
	{														\
		luaGetVariable(_l, _varname);						\
	}														\
	CLua::instance().checkAfter();						\
}

#define luaGetGlobalVector(_l,_varname)							\
{																\
	CLua::instance().checkBefore();							\
	lua_getglobal(_l, #_varname);                               \
	if (lua_isnil(_l, -1))                                      \
	{                                                           \
		nlwarning("luaGetGlobal : %s not found",#_varname);     \
		lua_pop(_l, 1);											\
	}                                                           \
	else                                                        \
	{															\
		luaGetVector(_l, _varname);								\
	}															\
	CLua::instance().checkAfter();							\
}

#define luaGetGlobalVectorWithName(_l,_var,_varname)        \
{															\
	CLua::instance().checkBefore();						\
	lua_getglobal(_l, _varname);                            \
	if (lua_isnil(_l, -1))                                  \
	{                                                       \
		nlwarning("luaGetGlobal : %s not found",#_varname); \
		lua_pop(_l, 1);										\
	}                                                       \
	else                                                    \
	{														\
		luaGetVector(_l, _var);								\
	}														\
	CLua::instance().checkAfter();						\
}

// Initialize a C variable with the lua variable on top of the stack

inline void luaGetVariable(lua_State *L, bool &var)
{
	var = lua_toboolean(L, -1)!=0;
	lua_pop(L, 1);
}

// this macro is used to set luaGetVariable for all number types
#define IMP_GET_VAR_INT(_a) \
inline void luaGetVariable(lua_State *L, _a &var)	\
{													\
	int isNumber = lua_isnumber(L,-1);				\
	if(isNumber)									\
	{												\
		var = (_a)lua_tonumber(L, -1);				\
	}												\
	else											\
	{												\
		nlwarning("luaGetVariable(not a number");	\
	}												\
	lua_pop(L, 1);									\
}

IMP_GET_VAR_INT(double);
IMP_GET_VAR_INT(float);
IMP_GET_VAR_INT(uint32);
IMP_GET_VAR_INT(uint16);
IMP_GET_VAR_INT(uint8);
IMP_GET_VAR_INT(sint32);
IMP_GET_VAR_INT(sint16);
IMP_GET_VAR_INT(sint8);

void luaGetVariable(lua_State *L, string &var);

template<class T>
void luaGetVector(lua_State *L, vector<T> &var)
{
	lua_pushnil(L);
	while(lua_next(L, -2) != 0)
	{
		// `key' is at index -2 and `value' at index -1
		T v;
		luaGetVariable(L, v);
		var.push_back(v);
	}
	lua_pop(L, 1);  // removes `key'
}

template<class T>
void luaGetVariable(lua_State *L, T &var)
{
	var = *Lunar<T>::check(L, -1);
	lua_pop(L, 1);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// This macro must be put in all class that use lunar
// It creates generic functions
#define LUA_BEGIN(ClassName) \
public: \
	ClassName(lua_State *L) { nlstop; } \
	static const char *className() { return #ClassName; } \
	bool methodExists(const string &func) \
	{ \
		return Lunar<ClassName>::methodExists(CLua::instance().l(), this, func.c_str()); \
	} \
	void call(const string &func, int nargs=0) \
	{ \
		H_AUTO(ClassName##LuaCall); \
		lua_State *L = CLua::instance().l(); \
		if (L == 0) return; \
		if (!Lunar<ClassName>::methodExists(L, this, func.c_str())) return; \
		if (nargs == 0) Lunar<ClassName>::push(L, this); \
		int res = Lunar<ClassName>::call(L, func.c_str(), nargs); \
		if (res == -1) \
		{ \
			string err = lua_tostring(L, -1); \
			if (err.empty()) err = "(error with no message)"; \
			nlerror ("Lunar<ClassName>::call error(%s) : (status = %d) %s", func.c_str(), res, err.c_str()); \
			return; \
		} \
	} \
	static Lunar<ClassName>::RegType *methods() \
	{ \
		static Lunar<ClassName>::RegType __m[] = \
		{

#define LUA_BIND(class, name) {#name, &class::name}

#define LUA_END \
			{ 0, 0 } \
		}; \
		return __m; \
	}

// example: LUA_BOOL_ACCESSOR(ok, Ok)
#define LUA_BOOL_ACCESSOR(func, var) \
private: int func(lua_State *L) \
{ \
	lua_pushboolean(L, int(func())); \
	return 1; \
} \
private: int set##var(lua_State *L) \
{ \
	int b = lua_toboolean(L,1); \
	set##var(bool(b!=0)); \
	lua_pop(L,1); \
	return 0; \
} \
public:

// example: LUA_NUMBER_ACCESSOR(sint32, score, Score)
#define LUA_NUMBER_ACCESSOR(type, func, var) \
private: \
	int func(lua_State *L) \
	{ \
		lua_pushnumber(L, lua_Number(func())); \
		return 1; \
	} \
	int set##var(lua_State *L) \
	{ \
		lua_Number n = lua_tonumber(L,1); \
		set##var(type(n)); \
		lua_pop(L,1); \
		return 0; \
	} \
public:

// example: LUA_STRING_ACCESSOR(shapeName, ShapeName)
#define LUA_STRING_ACCESSOR(func, var) \
private: \
	int func(lua_State *L) \
	{ \
		lua_pushstring(L, func().c_str()); \
		return 1; \
	} \
	int set##var(lua_State *L) \
	{ \
		const char *str = lua_tostring(L,1); \
		set##var(string(str)); \
		lua_pop(L,1); \
		return 0; \
	} \
public:

// example: LUA_UCSTRING_ACCESSOR(shapeName, ShapeName)
#define LUA_UCSTRING_ACCESSOR(func, var) \
private: \
	int func(lua_State *L) \
	{ \
		lua_pushstring(L, func().toUtf8().c_str()); \
		return 1; \
	} \
	int set##var(lua_State *L) \
	{ \
		const char *str = lua_tostring(L,1); \
		ucstring ucstr; \
		ucstr.fromUtf8(str); \
		set##var(ucstr); \
		lua_pop(L,1); \
		return 0; \
	} \
public:

#define LUA_ACCESSOR(type, func, var) \
private: \
	int func(lua_State *L) \
	{ \
		const type &v = func(); \
		Lunar<type>::push(L, &v); \
		return 1; \
	} \
	int set##var(lua_State *L) \
	{ \
		const type *v = Lunar<type>::check(L, 1); \
		set##var(*v); \
		lua_pop(L,1); \
		return 0; \
	} \
public:

// define the string, accessors and lua accessors
// example STRING_ACCESSOR(name, Name);
#define STRING_ACCESSOR(func, var) \
private: string var; \
public: virtual const string &func() const { return var; } \
public: virtual void set##var(const string &nvar, bool display=true) { if(var != nvar && display) { nlinfo("LEVEL: set%s changes from %s to %s", #var, var.c_str(), nvar.c_str()); } var = nvar; } \
private: int func(lua_State *L) \
{ \
	lua_pushstring(L, func().c_str()); \
	return 1; \
} \
private: int set##var(lua_State *L) \
{ \
	const char *str = lua_tostring(L,1); \
	bool display = lua_isboolean(L,2) ? (lua_toboolean(L,2)==1) : true; \
	set##var(string(str), display); \
	lua_pop(L,1); \
	return 0; \
} \
public:

// define the number, c++ accessors and lua accessors
// example: NUMBER_ACCESSOR(uint8, bounce, Bounce)
#define NUMBER_ACCESSOR(type, func, var) \
private: type var; \
public: virtual type func() const { return var; } \
public: virtual void set##var(type nvar, bool display=true) { if(var != nvar && display) { nlinfo("LEVEL: set%s changes from %s to %s", #var, NLMISC::toString(type(var)).c_str(), NLMISC::toString(type(nvar)).c_str()); } var = nvar; } \
private: int func(lua_State *L) \
{ \
	lua_pushnumber(L, lua_Number(func())); \
	return 1; \
} \
private: int set##var(lua_State *L) \
{ \
	lua_Number n = lua_tonumber(L,1); \
	bool display = lua_isboolean(L,2) ? (lua_toboolean(L,2)==1) : true; \
	set##var(type(n), display); \
	lua_pop(L,1); \
	return 0; \
} \
public:

// define the bool, accessors and lua accessors
// example: BOOL_ACCESSOR(ok, Ok)
#define BOOL_ACCESSOR(func, var) \
private: bool var; \
public: virtual bool func() const { return var; } \
public: virtual void set##var(bool nvar, bool display=true) { if(var != nvar && display) { nlinfo("LEVEL: set%s changes from %s to %s", #var, (var?"true":"false"), (nvar?"true":"false")); } var = nvar; } \
private: int func(lua_State *L) \
{ \
	lua_pushboolean(L, int(func())); \
	return 1; \
} \
private: int set##var(lua_State *L) \
{ \
	int b = lua_toboolean(L,1); \
	bool display = lua_isboolean(L,2) ? (lua_toboolean(L,2)==1) : true; \
	set##var(bool(b!=0), display); \
	lua_pop(L,1); \
	return 0; \
} \
public:

#define ACCESSOR(type, func, var) \
private: type var; \
public: virtual type func() const { return var; } \
public: virtual void set##var(type nvar, bool display=true) { if(var != nvar && display) { nlinfo("LEVEL: set%s changes", #var); } var = nvar; } \
private: \
	int func(lua_State *L) \
	{ \
		const type &v = func(); \
		Lunar<type>::push(L, &v); \
		return 1; \
	} \
	int set##var(lua_State *L) \
	{ \
		const type *v = Lunar<type>::check(L, 1); \
		set##var(*v); \
		lua_pop(L,1); \
		return 0; \
	} \
public:

#endif
