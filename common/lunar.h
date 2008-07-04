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

#ifndef MT_LUNAR_H
#define MT_LUNAR_H


//
// Includes
//

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

template <typename T> class Lunar {
  typedef struct { T *pT; } userdataType;
public:
  typedef int (T::*mfp)(lua_State *L);
  typedef struct { const char *name; mfp mfunc; } RegType;

  static void registerMethods(lua_State *L) {
    lua_newtable(L);
    int methods = lua_gettop(L);

    luaL_newmetatable(L, T::className());
    int metatable = lua_gettop(L);

    // store method table in globals so that
    // scripts can add functions written in Lua.
    lua_pushvalue(L, methods);
    set(L, LUA_GLOBALSINDEX, T::className());

    // hide metatable from Lua getmetatable()
    //lua_pushvalue(L, methods);
    //set(L, metatable, "__metatable");

    lua_pushvalue(L, methods);
    set(L, metatable, "__index");

    lua_pushcfunction(L, tostring_T);
    set(L, metatable, "__tostring");

    lua_pushcfunction(L, gc_T);
    set(L, metatable, "__gc");

    lua_newtable(L);                // mt for method table
    lua_pushcfunction(L, new_T);
    lua_pushvalue(L, -1);           // dup new_T function
    set(L, methods, "new");         // add new_T to method table
    set(L, -3, "__call");           // mt.__call = new_T
    lua_setmetatable(L, methods);

    // fill method table with methods from class T
    for (RegType *l = T::methods(); l->name; l++) {
      lua_pushstring(L, l->name);
      lua_pushlightuserdata(L, (void*)l);
      lua_pushcclosure(L, thunk, 1);
      lua_settable(L, methods);
    }

    lua_pop(L, 2);  // drop metatable and method table
  }


  bool static methodExists(lua_State *L, T *obj, const char *method)
  {
	  H_AUTO2;
	  if(!L) return false;
	  bool res;
	  push(L,obj);
	  int base = lua_gettop(L);			// userdata index
	  lua_pushstring(L, method);        // method name
	  lua_gettable(L, base);            // get method from userdata
	  res = lua_isnil(L, -1)==0;
	  lua_settop(L, base-1);			// drop userdata and args
	  return res;
  }

  // call named lua method from userdata method table
  static int call(lua_State *L, const char *method,
                  int nargs=0, int nresults=LUA_MULTRET, int errfunc=0)
  {
	H_AUTO2;
    int base = lua_gettop(L) - nargs;  // userdata index
    if (!luaL_checkudata(L, base, T::className())) {
      lua_settop(L, base-1);           // drop userdata and args
      lua_pushfstring(L, "not a valid %s userdata", T::className());
      return -1;
    }

    lua_pushstring(L, method);         // method name
    lua_gettable(L, base);             // get method from userdata
    if (lua_isnil(L, -1)) {            // no method?
      lua_settop(L, base-1);           // drop userdata and args
      lua_pushfstring(L, "%s missing method '%s'", T::className(), method);
      return -1;
    }
    lua_insert(L, base);               // put method under userdata, args

	H_BEFORE(CLuaCallPCall);
    int status = lua_pcall(L, 1+nargs, nresults, errfunc);  // call method
	H_AFTER(CLuaCallPCall);
	if (status) {
      const char *msg = lua_tostring(L, -1);
      string fm;
	  if (msg == 0) fm = "(error with no message)";
	  else
	  {
		  char *f2 = 0, *f = strchr((char*)msg, ':');
		  if(f != 0)
		  {
			  f2 = strchr(f+1, ':');
		  }
		  if(f && f2)
		  {
			  *f = '(';
			  *f2 = ')';
			  fm = msg;
			  fm.insert((f2-msg+1), " :");
		  }
	  }
      lua_pushfstring(L, "%s:%s status = %d\n%s", T::className(), method, status, fm.c_str());
      lua_remove(L, base);             // remove old message
      return -1;
    }
    return lua_gettop(L) - base + 1;   // number of results
  }

  // push onto the Lua stack a userdata containing a pointer to T object
  static int push(lua_State *L, const T *obj, bool gc=false) {
	H_AUTO2;
    if (!obj) { lua_pushnil(L); return 0; }
    luaL_getmetatable(L, T::className());  // lookup metatable in Lua registry
    if (lua_isnil(L, -1)) luaL_error(L, "%s missing metatable", T::className());
    int mt = lua_gettop(L);
    subtable(L, mt, "userdata", "v");
    userdataType *ud = static_cast<userdataType*>(pushuserdata(L, (void*)obj, sizeof(userdataType)));
    if (ud) {
      ud->pT = (T*)obj;  // store pointer to object in userdata
      lua_pushvalue(L, mt);
      lua_setmetatable(L, -2);
      if (gc == false) {
        lua_checkstack(L, 3);
        subtable(L, mt, "do not trash", "k");
        lua_pushvalue(L, -2);
        lua_pushboolean(L, 1);
        lua_settable(L, -3);
        lua_pop(L, 1);
      }
    }
    lua_replace(L, mt);
    lua_settop(L, mt);
    return mt;  // index of userdata containing pointer to T object
  }

  // get userdata from Lua stack and return pointer to T object
  static T *check(lua_State *L, int narg) 
  {
    userdataType *ud = static_cast<userdataType*>(luaL_checkudata(L, narg, T::className()));
    if(!ud) luaL_typerror(L, narg, T::className());
    return ud->pT;  // pointer to T object
  }

private:
  Lunar();  // hide default constructor

  // member function dispatcher
  static int thunk(lua_State *L) {
    // stack has userdata, followed by method args
    T *obj = (T*)check(L, 1);  // get 'self', or if you prefer, 'this'
    lua_remove(L, 1);  // remove self so member function args start at index 1
    // get member function from upvalue
    RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
    return (obj->*(l->mfunc))(L);  // call member function
  }

  // create a new T object and
  // push onto the Lua stack a userdata containing a pointer to T object
  static int new_T(lua_State *L) {
    lua_remove(L, 1);   // use classname:new(), instead of classname.new()
    T *obj = new T(L);  // call constructor for T objects
    push(L, obj, true); // gc_T will delete this object
    return 1;           // userdata containing pointer to T object
  }

  // garbage collection metamethod
  static int gc_T(lua_State *L) {
    if (luaL_getmetafield(L, 1, "do not trash")) {
      lua_pushvalue(L, 1);  // dup userdata
      lua_gettable(L, -2);
      if (!lua_isnil(L, -1)) return 0;  // do not delete object
    }
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    if (obj) delete obj;  // call destructor for T objects
    return 0;
  }

  static int tostring_T (lua_State *L) {
    char buff[32];
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    sprintf(buff, "%p", obj);
    lua_pushfstring(L, "%s (%s)", T::className(), buff);
    return 1;
  }

  static void set(lua_State *L, int table_index, const char *key) {
    lua_pushstring(L, key);
    lua_insert(L, -2);  // swap value and key
    lua_settable(L, table_index);
  }

  static void weaktable(lua_State *L, const char *mode) {
    lua_newtable(L);
    lua_pushvalue(L, -1);  // table is its own metatable
    lua_setmetatable(L, -2);
    lua_pushliteral(L, "__mode");
    lua_pushstring(L, mode);
    lua_settable(L, -3);   // metatable.__mode = mode
  }

  static void subtable(lua_State *L, int tindex, const char *name, const char *mode) {
    lua_pushstring(L, name);
    lua_gettable(L, tindex);
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);
      lua_checkstack(L, 3);
      weaktable(L, mode);
      lua_pushstring(L, name);
      lua_pushvalue(L, -2);
      lua_settable(L, tindex);
    }
  }

  static void *pushuserdata(lua_State *L, void *key, size_t sz) {
    void *ud = 0;
    lua_pushlightuserdata(L, key);
    lua_gettable(L, -2);     // lookup[key]
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);         // drop nil
      lua_checkstack(L, 3);
      ud = lua_newuserdata(L, sz);  // create new userdata
      lua_pushlightuserdata(L, key);
      lua_pushvalue(L, -2);  // dup userdata
      lua_settable(L, -4);   // lookup[key] = userdata
    }
    return ud;
  }
};

#define bind_method(class, name) {#name, &class::name}

#endif
