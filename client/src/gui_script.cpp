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

#include "gui_script.h"
#include "3d_task.h"
#include "time_task.h"
#include "gui_bitmap.h"
#include "gui_stretched_quad.h"
#include "gui_text.h"
#include "gui_xml.h"

#include <nel/3d/u_material.h>


//
// Namespaces
//

using namespace NL3D;


//
// Functions
//

void CGuiScriptManager::init()
{
	CGuiScript::xmlRegister();	
}


void CGuiScriptManager::render()
{
	
	
}

void CGuiScriptManager::release()
{
	
}


//
//
//
CGuiScript::CGuiScript()
{
}

CGuiScript::~CGuiScript()
{
	
}

void CGuiScript::load(lua_State *luaState,const string &filename)
{
	luaLoad(luaState,filename);
}


void CGuiScript::xmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiScript",CGuiScript::create);
}

CGuiObject *CGuiScript::create()
{
	CGuiObject *res = new CGuiScript;
	
	return res;	
}

void CGuiScript::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiObject::init(xml,node);
	string filename;
	bool isok;

	isok = xml->getString(node,"filename",filename);
	nlassert(isok);
	load(xml->LuaState,filename);
}

#endif
