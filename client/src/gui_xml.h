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

#ifndef MT_GUI_XML_H
#define MT_GUI_XML_H


//
// Includes
//

#include <nel/misc/singleton.h>
#include <nel/misc/i_xml.h>

#include "gui_object.h"


//
// Classes
//

class CGuiXml
{
public:
	CGuiObject *get(const std::string &name);
	CGuiObject *getRoot();
	CGuiObject *get(uint index);
	uint count();

	CGuiXml(const std::string &fn);
	virtual ~CGuiXml();

	bool getVector(xmlNodePtr node,const string &name,NLMISC::CVector &res);
	bool getAlignment(xmlNodePtr node,const string &name,CGuiObject::TGuiAlignment &res);
	bool getString(xmlNodePtr node,const string &name,string &res);
	bool getBool(xmlNodePtr node,const string &name,bool &res);
		
	friend class CGuiXmlManager;
	list<guiSPG<CGuiObject> > objects;
	guiSPG<CGuiObject> root;
	NLMISC::CIXml doc;
	lua_State				*LuaState;
	std::string			FileName;
};

class CGuiXmlManager : public NLMISC::CSingleton<CGuiXmlManager>
{
public:
	static CGuiXml *load(const string &filename, lua_State *luaState=0);
};

#endif
