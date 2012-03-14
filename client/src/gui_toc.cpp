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

#include "3d_task.h"
#include "chat_task.h"
#include "time_task.h"
#include "resource_manager2.h"
#include "gui_toc.h"
#include "gui_xml.h"

#include "entity_lua_proxy.h"
#include "level.h"
#include "gui.h"
#include "network_task.h"



//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	
CGuiToc::CGuiToc()
{
	_element = 0;

	LuaState = luaOpen();
	Lunar<CEntityProxy>::Register(LuaState);
	Lunar<CModuleProxy>::Register(LuaState);
	Lunar<CParticlesProxy>::Register(LuaState);

	Lunar<CGuiObject>::Register(LuaState);
	Lunar<CGuiListView>::Register(LuaState);
	Lunar<CGuiBox>::Register(LuaState);
	Lunar<CGuiText>::Register(LuaState);

	lua_register(LuaState, "getEntityByName", CLevel::getEntityByName);	
	lua_register(LuaState, "getEntityById", CLevel::getEntityById);
	lua_register(LuaState, "getGuiElementByName", CGuiToc::getGuiElementByName);
	lua_register(LuaState, "sendChat", CGuiToc::sendChat);
	lua_register(LuaState, "sendCommand", CGuiToc::sendCommand);
	lua_register(LuaState, "sendToConsole", CGuiToc::sendToConsole);
	lua_register(LuaState, "loadXml", CGuiToc::loadXml);

}

CGuiToc::~CGuiToc()
{
	_element = 0;
}


void CGuiToc::update()
{
	if(LuaState)
	{
		lua_getglobal(LuaState, "update");
		if (lua_isnil(LuaState, -1))
		{
			lua_pop(LuaState, 1);
			return;
		}
		int res = lua_pcall (LuaState,0,0,0);
		if (res) 
		{
			const char *msg = lua_tostring(LuaState, -1);
			if (msg == 0) msg = "(error with no message)";
			//if(warning())
				nlwarning("pcall error(update) : (status = %d)%s", res, msg);
			return;
		}
	}
}

void CGuiToc::render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize)
{
	xml->getRoot()->render(pos,maxSize);
}


void CGuiToc::onLogin(const string &name)
{
	if(LuaState)
	{
		lua_getglobal(LuaState, "onLogin");
		if (lua_isnil(LuaState, -1))
		{
			lua_pop(LuaState, 1);
			return;
		}
		
		lua_pushstring(LuaState,name.c_str());
		int res = lua_pcall (LuaState,1,0,0);
		if (res) 
		{
			const char *msg = lua_tostring(LuaState, -1);
			if (msg == 0) msg = "(error with no message)";
			//if(warning())
				nlwarning("pcall error(onLogin) : (status = %d)%s", res, msg);
			return;
		}
	}
}

void CGuiToc::onLogout(const string &name)
{
	if(LuaState)
	{
		lua_getglobal(LuaState, "onLogout");
		if (lua_isnil(LuaState, -1))
		{
			lua_pop(LuaState, 1);
			return;
		}
		
		lua_pushstring(LuaState,name.c_str());
		int res = lua_pcall (LuaState,1,0,0);
		if (res) 
		{
			const char *msg = lua_tostring(LuaState, -1);
			if (msg == 0) msg = "(error with no message)";
			//if(warning())
				nlwarning("pcall error(onLogout) : (status = %d)%s", res, msg);
			return;
		}
	}
}

CGuiToc *CGuiToc::Load(const string &filename)
{

	nlinfo("Loading TOC : %s",filename.c_str());

	xmlNodePtr node;
	CGuiToc *res;
	CIFile file;
	if (file.open(CPath::lookup(filename, false).c_str()))
	{
		res = new CGuiToc;
		if (res->doc.init(file))
		{
			node = res->doc.getRootNode();
		}		
		else
		{
			nlassert(false);
			delete res;
			return 0;
		}
	}
	else
	{
		nlassert(false);
		return 0;
	}

	for( node = res->doc.getFirstChildNode(node,"gui");node;node = res->doc.getNextChildNode(node,"gui") )
	{
		string xmlFilename;
		const char *value = (const char*)xmlGetProp (node, (xmlChar*)"filename");
		if (value)
		{
			xmlFilename = value;
			nlinfo("found : %s",xmlFilename.c_str());
			res->xml = CGuiXmlManager::Load(xmlFilename,res->LuaState);
			
			//CGuiObjectManager::getInstance().objects.push_back(res->xml->getRoot());
			break;
		}
		//filenameList.push_back(filename);
	}


	return res;
}

int CGuiToc::sendChat(lua_State *L)
{
	size_t len;
	const char *charChatLine= luaL_checklstring(L, 1, &len);
	string chatLine(charChatLine);
	CNetworkTask::getInstance().chat(chatLine);
	return 0;
}

int CGuiToc::sendCommand(lua_State *L)
{
	size_t len;
	const char *cCommandLine= luaL_checklstring(L, 1, &len);
	string cmdLine(cCommandLine);
	CNetworkTask::getInstance().command(cmdLine);
	return 0;
}

int CGuiToc::sendToConsole(lua_State *L)
{
	size_t len;
	const char *cText= luaL_checklstring(L, 1, &len);
	string text(cText);
	CChatTask::getInstance().addToInput(text);
	return 0;
}

int CGuiToc::getGuiElementByName(lua_State *L)
{
	size_t len;
	const char *elementName = luaL_checklstring(L, 1, &len);
	string name(elementName);
	CGuiObject *object = CGuiCustom::getInstance().getTocByLuaState(L)->xml->get(name);
	if(object==0)
	{
		nlwarning("getGuiElementByName(%s)==0",name.c_str());
		return 0;
	}
	object->luaPush(L);
	return 1;
}

int CGuiToc::loadXml(lua_State *L)
{
	size_t len;
	const char *cfileName = luaL_checklstring(L, 1, &len);
	string filename(cfileName);

	CGuiXml *xml = CGuiXmlManager::Load(filename,L);
	if(xml)
	{
		xml->getRoot()->luaPush(L);
		return 1;
	}

	return 0;
}


