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

#ifndef _LUA_ENGINE_H_
#define _LUA_ENGINE_H_


//
// Includes
//

#include <nel/misc/singleton.h>

#include "../../common/lua_utility.h"
#include "../../common/lunar.h"
#include "entity_lua_proxy.h"
#include "module_lua_proxy.h"
#include "level_lua_proxy.h"


//
// Classes
//

class CLuaEngine : public NLMISC::CSingleton<CLuaEngine>
{
public:
	CLuaEngine();
	void init(const std::string &filename);
	void update();
	void release();
	void error();
	bool hasError() {return _error;}
	bool warning() {_warning++; return _warning<=_warningMaxCount;}
	

	void entityEntityCollideEvent(CEntity *client1, CEntity *client2);
	void entitySceneCollideEvent(CEntity *client, CModule *module);
	void entityWaterCollideEvent(CEntity *client);
	void entityLeaveEvent(CEntity *client);
	
	void levelInit();
	void levelPreUpdate();
	void levelPostUpdate();
	void levelEndSession();
		
	lua_State *session();

private:
	static int lua_ALERT(lua_State *L);

	static int getSessionId(lua_State *L);
	static int getLevelSessionCount(lua_State *L);
	static int setMaxLevelSessionCount(lua_State *L);

	static int getEntityCount(lua_State *L);
	static int getEntity(lua_State *L);
	static int getEntityById(lua_State *L);
	static int getModuleCount(lua_State *L);
	static int getModule(lua_State *L);
	static int sendChat(lua_State *L);
	static int displayTextToAll(lua_State *L);
	static int setLevelHasBonusTime(lua_State *L);
	static int setLevelRecordBest(lua_State *L);
	static int setLevelTimeout(lua_State *L);
	static int getLevelTimeout(lua_State *L);
	static int getTimeRemaining(lua_State *L);

	static int execLuaOnAllClient(lua_State *L);
	static int execLuaOnOneClient(lua_State *L);
	static int execLuaOnAllButOneClient(lua_State *L);
	
	static uint32 _sessionId;
		
	lua_State *_luaSession;
	CLevel *_level;
	bool _error;
	uint32 _warning;
	uint32 _warningMaxCount;
	
};




#endif
