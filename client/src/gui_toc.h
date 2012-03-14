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


#ifndef MTPT_GUI_TOC_H
#define MTPT_GUI_TOC_H


//
// Includes
//
#include "gui_object.h"
#include "gui_spg.h"
#include <libxml/parser.h>
#include <nel/misc/i_xml.h>
#include <vector>
#include <list>



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

	static CGuiToc *Load(const std::string &filename);
	static int getGuiElementByName(lua_State *L);
	static int sendChat(lua_State *L);
	static int sendCommand(lua_State *L);
	static int sendToConsole(lua_State *L);
	static int loadXml(lua_State *L);

	void onLogin(const std::string &name);
	void onLogout(const std::string &name);

	NLMISC::CIXml doc;
	CGuiXml *xml;
	lua_State				*LuaState;
private:
	guiSPG<CGuiObject> _element;
	std::list<std::string> filenameList;
};


#endif
