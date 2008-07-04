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
// Vianney Lecroart - acemtp@vialek.com


//
// Includes
//

#include "stdpch.h"

#if 0

#include "gui.h"
#include "level.h"
#include "module.h"
#include "3d_task.h"
#include "gui_toc.h"
#include "gui_xml.h"
#include "chat_task.h"
#include "time_task.h"
#include "particles.h"
#include "network_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;

//
// Functions
//

CGuiToc::CGuiToc()
{
	_element = 0;

	LuaState = luaOpen();
	Lunar<CEntity>::registerMethods(LuaState);
	Lunar<CModule>::registerMethods(LuaState);
	Lunar<CParticles>::registerMethods(LuaState);

	Lunar<CGuiObject>::registerMethods(LuaState);
	Lunar<CGuiListView>::registerMethods(LuaState);
	Lunar<CGuiBox>::registerMethods(LuaState);
	Lunar<CGuiText>::registerMethods(LuaState);

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


void CGuiToc::onLogin(const ucstring &name)
{
	if(LuaState)
	{
		lua_getglobal(LuaState, "onLogin");
		if (lua_isnil(LuaState, -1))
		{
			lua_pop(LuaState, 1);
			return;
		}
		
		lua_pushstring(LuaState,name.toUtf8().c_str());
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

void CGuiToc::onLogout(const ucstring &name)
{
	if(LuaState)
	{
		lua_getglobal(LuaState, "onLogout");
		if (lua_isnil(LuaState, -1))
		{
			lua_pop(LuaState, 1);
			return;
		}
		
		lua_pushstring(LuaState,name.toUtf8().c_str());
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
			delete res;
			res = 0;
			nlstop;
			return 0;
		}
	}
	else
	{
		nlstop;
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
	unsigned int len;
	const char *charChatLine= luaL_checklstring(L, 1, &len);
	string chatLine(charChatLine);
	CNetworkTask::getInstance().chat(chatLine);
	return 0;
}

int CGuiToc::sendCommand(lua_State *L)
{
	unsigned int len;
	const char *cCommandLine= luaL_checklstring(L, 1, &len);
	string cmdLine(cCommandLine);
	CNetworkTask::getInstance().command(cmdLine);
	return 0;
}

int CGuiToc::sendToConsole(lua_State *L)
{
	unsigned int len;
	const char *cText= luaL_checklstring(L, 1, &len);
	string text(cText);
	CChatTask::getInstance().addToInput(text);
	return 0;
}

int CGuiToc::getGuiElementByName(lua_State *L)
{
	unsigned int len;
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
	unsigned int len;
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

#endif // 0
