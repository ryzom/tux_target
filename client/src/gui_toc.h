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

#ifndef MTPT_GUI_TOC_H
#define MTPT_GUI_TOC_H

#if 0

//
// Includes
//

#include "gui_object.h"
#include "gui_spg.h"
#include <libxml/parser.h>
#include <nel/misc/i_xml.h>


//
// Classes
//

class CGuiToc
{
public:
	CGuiToc();
	virtual ~CGuiToc();
	virtual void update();
	virtual void render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);

	static CGuiToc *Load(const string &filename);
	static int getGuiElementByName(lua_State *L);
	static int sendChat(lua_State *L);
	static int sendCommand(lua_State *L);
	static int sendToConsole(lua_State *L);
	static int loadXml(lua_State *L);

	void onLogin(const ucstring &name);
	void onLogout(const ucstring &name);



	NLMISC::CIXml doc;
	CGuiXml *xml;
	lua_State				*LuaState;
private:
	guiSPG<CGuiObject> _element;
	list<string> filenameList;
};

#endif // 0

#endif
