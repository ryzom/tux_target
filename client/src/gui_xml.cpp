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

#include "resource_manager2.h"
#include "gui_xml.h"
#include "gui.h"


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
	

CGuiObject *CGuiXml::get(string name)
{
	CGuiObject *obj;
	list<guiSPG<CGuiObject> >::iterator it;
	for(it=objects.begin();it!=objects.end();it++)
	{
		obj = *it;
		if(obj->name()==name)
			return obj;
	}
	return 0;
}

CGuiObject *CGuiXml::getRoot()
{
	return root;
}

CGuiObject *CGuiXml::get(uint index)
{
	uint i;
	CGuiObject *obj;
	list<guiSPG<CGuiObject> >::iterator it;
	for(i=0,it=objects.begin();it!=objects.end();it++,i++)
	{
		obj = *it;
		if(i==index)
			return obj;
	}
	return 0;
}

uint CGuiXml::count()
{
	return objects.size();
}



CGuiXml::CGuiXml()
{
	LuaState = 0;
}

CGuiXml::~CGuiXml()
{
	objects.clear();
}



CGuiXml *CGuiXmlManager::Load(const string &filename, lua_State *luaState)
{

	xmlNodePtr node;
	CGuiXml *res;
	CIFile file;
	if (file.open(CPath::lookup(filename, false).c_str()))
	{
		res = new CGuiXml;
		res->LuaState = luaState;
		if (res->doc.init(file))
		{
			node = res->doc.getRootNode();
//			node = res->doc.getFirstChildNode(node,"resource");
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


/*
	TiXmlDocument doc( filename.c_str() );
	bool loadOkay = doc.LoadFile(CPath::lookup(filename, false).c_str());
	nlassert(loadOkay);


	TiXmlNode* node = 0;
	TiXmlElement* itemElement = 0;
	node = doc.FirstChild( "resource" );
	nlassert( node );
	itemElement = node->ToElement();
	nlassert( itemElement  );
*/	
	
	for( node = res->doc.getFirstChildNode(node,"object");node;node = res->doc.getNextChildNode(node,"object") )
	{
		CGuiObject *object = CGuiObject::XmlCreateFromNode(res,node);
		res->objects.push_back(object);
		res->root = object;
	}

	


	return res;
}


bool CGuiXml::getVector(xmlNodePtr node,const string &name,CVector &res)
{
	node = doc.getFirstChildNode(node,name.c_str());
	if(node)
	{
		res = CVector(0,0,0);
		string s;
		doc.getContentString(s,node);
		char tmpstr[1024];
		strcpy(tmpstr,s.c_str());
		char *cstr = tmpstr;
		char *endstr = 0;
		res.x = (float)strtod(cstr,&endstr);
		if(endstr[0]==',')
		{
			cstr = endstr+1;
			res.y = (float)strtod(cstr,&endstr);
		}
		if(endstr[0]==',')
		{
			cstr = endstr+1;
			res.z = (float)strtod(cstr,&endstr);
		}
		return true;
	}
	
	return false;
}

bool CGuiXml::getAlignment(xmlNodePtr node,const std::string &name,CGuiObject::TGuiAlignment &res)
{
	node = doc.getFirstChildNode(node,name.c_str());
	if(node)
	{
		res = CGuiObject::eAlignNone;
		string s;
		doc.getContentString(s,node);
		if(s.find("eAlignCenterVertical")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignCenterVertical);
		if(s.find("eAlignCenterHorizontal")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignCenterHorizontal);
		if(s.find("eAlignUp")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignUp);
		if(s.find("eAlignBottom")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignBottom);
		if(s.find("eAlignLeft")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignLeft);
		if(s.find("eAlignRight")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignRight);
		if(s.find("eAlignExpandVertical")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignExpandVertical);
		if(s.find("eAlignExpandHorizontal")!=string::npos)
			res = (CGuiObject::TGuiAlignment)(res | CGuiObject::eAlignExpandHorizontal);

		return true;
	}
	return false;
}


bool CGuiXml::getString(xmlNodePtr node,const std::string &name,std::string &res)
{
	node = doc.getFirstChildNode(node,name.c_str());
	if(node)
	{
		string s;
		doc.getContentString(s,node);
		res = s;
		return true;
	}
	return false;
}

bool CGuiXml::getBool(xmlNodePtr node,const std::string &name,bool &res)
{
	node = doc.getFirstChildNode(node,name.c_str());
	if(node)
	{
		string s;
		doc.getContentString(s,node);
		res = s.find("true")!=string::npos || s.find("1")!=string::npos;
		return true;
	}
	return false;
}

