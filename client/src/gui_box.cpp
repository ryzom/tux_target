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

// Includes
//

#include "stdpch.h"

#include "3d_task.h"
#include "time_task.h"
#include "resource_manager2.h"
#include "gui_box.h"
#include <nel/3d/u_material.h>
#include "gui_xml.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//

const char CGuiBox::className[] = "CGuiBox";

Lunar<CGuiBox>::RegType CGuiBox::methods[] = 
{
	bind_method(CGuiBox, getName),	
	bind_method(CGuiBox, getCount),	
	bind_method(CGuiBox, getElement),	
	{0,0}
};


int CGuiBox::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession,name().c_str());
	return 1;
}

int CGuiBox::getCount(lua_State *luaSession)
{
	lua_Number count = elements.size();
	lua_pushnumber(luaSession,count);
	return 1;
}

int CGuiBox::getElement(lua_State *luaSession)
{
	lua_Number argIndex = luaL_checknumber(luaSession,1);
	uint index = (uint)argIndex;
	guiSPG<CGuiObject> item = elements[index];
	item->luaPush(luaSession);
	return 1;
}



//
// Functions
//
	

void CGuiBoxManager::init()
{
	
	string res;
	res = CResourceManager::getInstance().get("box_debug.tga");
	_texture = C3DTask::getInstance().driver().createTextureFile(res);
	nlassert(_texture);
	
	//_texture->setWrapS(UTexture::Clamp);
	//_textureFrame->setWrapT(UTexture::Clamp);
	
	
	_material = C3DTask::getInstance().createMaterial();
	_material.setTexture(_texture);
	_material.setBlend(true);
	_material.setZFunc(UMaterial::always);
	_material.setDoubleSided();

	CGuiHBox::XmlRegister();
	CGuiVBox::XmlRegister();
	
}
	
void CGuiBoxManager::render()
{
}

void CGuiBoxManager::release()
{
	
}

NL3D::UTextureFile	*CGuiBoxManager::texture()
{
	return _texture;
}

NL3D::UMaterial CGuiBoxManager::material()
{
	return _material;
}



//
//
//
CGuiBox::CGuiBox()
{
	_spacing = 2;
	quad.material(CGuiBoxManager::getInstance().material());
}

CGuiBox::~CGuiBox()
{
	elements.clear();
}

float CGuiBox::spacing()
{
	return _spacing;
}

void CGuiBox::spacing(float spacing)
{
	_spacing = spacing;
}

CGuiObject::TGuiAlignment CGuiBox::alignment()
{
	CGuiObject::TGuiAlignment res = CGuiObject::alignment();
	deque<guiSPG<CGuiObject> >::iterator it;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		if(obj->alignment()&CGuiObject::eAlignExpandHorizontal)
			res = (TGuiAlignment )(res | eAlignExpandHorizontal);
		if(obj->alignment()&CGuiObject::eAlignExpandVertical)
			res = (TGuiAlignment )(res | eAlignExpandVertical);
	}
	
	return res;
}

void CGuiBox::alignment(int alignment)
{
	CGuiObject::alignment(alignment);
}


void CGuiBox::luaPush(lua_State *L)
{
	Lunar<CGuiBox>::push(L, this);
}


void CGuiBox::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiContainer::init(xml,node);
	node = xml->doc.getFirstChildNode(node,"element");
	if(node)
		for( node = xml->doc.getFirstChildNode(node,"object");node;node = xml->doc.getNextChildNode(node,"object") )
		{
			CGuiObject *object = CGuiObject::XmlCreateFromNode(xml,node);
			elements.push_back(object);
			xml->objects.push_back(object);
		}
}



//
//
//
CGuiVBox::CGuiVBox()
{
}

CGuiVBox::~CGuiVBox()
{
	
}

void CGuiVBox::_render(const CVector &pos,CVector &maxSize)
{
	if(elements.empty()) return;
	CVector globalPos = globalPosition(pos,maxSize);
	maxSize = expandSize(maxSize);

	deque<guiSPG<CGuiObject> >::iterator it;
	float expandableHeight = maxSize.y;
	bool expandableObjectPresent = false;
	float centerCount = 0;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		expandableHeight -= obj->height() + spacing();
		if(obj->alignment()&CGuiObject::eAlignExpandVertical)
			expandableObjectPresent = true;
		if(obj->alignment()&CGuiObject::eAlignCenterVertical)
			centerCount++;
	}
	expandableHeight += spacing();
	float centerSpacing = 0;
	if(!expandableObjectPresent && centerCount>0)
		centerSpacing = expandableHeight / centerCount;

	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		float objHeight = obj->height();
		CVector newMaxSize(maxSize.x,0,0);
		if(expandableObjectPresent)
		{
			if(obj->alignment()&CGuiObject::eAlignExpandVertical)
				newMaxSize.y = expandableHeight + objHeight;
			else
				newMaxSize.y = objHeight;
		}
		else
		{
			if(obj->alignment()&CGuiObject::eAlignCenterVertical)
				newMaxSize.y = objHeight + centerSpacing;
			else
				newMaxSize.y = objHeight;
			if(obj->alignment()&CGuiObject::eAlignBottom)
				globalPos.y += expandableHeight;
		}
		obj->render(globalPos,newMaxSize);
		if(obj->alignment()&CGuiObject::eAlignCenterVertical)
			newMaxSize.y = objHeight + centerSpacing;
		globalPos.y += newMaxSize.y + spacing();
		expandableHeight -= newMaxSize.y - objHeight; 
		if(expandableHeight<0)
			nlwarning("expandableHeight(%f)<0",expandableHeight);
	}
}

float CGuiVBox::_width()
{
	deque<guiSPG<CGuiObject> >::iterator it;
	float res = 0;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		if(res<obj->width())
			res = obj->width();
	}	
	return res;
}

float CGuiVBox::_height()
{
	deque<guiSPG<CGuiObject> >::iterator it;
	float res = 0;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		res += obj->height();
	}	
	if(elements.size()>1)
		return res + (elements.size()-1) * spacing();	
	else
		return res;
}

void CGuiVBox::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiVBox",CGuiVBox::Create);
}

CGuiObject *CGuiVBox::Create()
{
	CGuiObject *res = new CGuiVBox;
	
	return res;	
}

void CGuiVBox::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiBox::init(xml,node);
}



//
//
//
CGuiHBox::CGuiHBox()
{
}

CGuiHBox::~CGuiHBox()
{
	
}

void CGuiHBox::_render(const CVector &pos,CVector &maxSize)
{
	if(elements.empty()) return;
	CVector globalPos = globalPosition(pos,maxSize);
	maxSize = expandSize(maxSize);
	
	
	deque<guiSPG<CGuiObject> >::iterator it;
	float expandableWidth= maxSize.x;
	bool expandableObjectPresent = false;
	float centerCount = 0;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		expandableWidth -= obj->width() + spacing();
		if(obj->alignment()&CGuiObject::eAlignExpandHorizontal)
			expandableObjectPresent = true;
		if(obj->alignment()&CGuiObject::eAlignCenterHorizontal)
			centerCount++;
	}
	expandableWidth += spacing();
	float centerSpacing = 0;
	if(!expandableObjectPresent && centerCount>0)
		centerSpacing = expandableWidth / centerCount;
	
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		float objWidth = obj->width();
		CVector newMaxSize(0,maxSize.y,0);
		if(expandableObjectPresent)
		{
			if(obj->alignment()&CGuiObject::eAlignExpandHorizontal)
				newMaxSize.x = expandableWidth + objWidth;
			else
				newMaxSize.x = objWidth;
		}
		else
		{
			if(obj->alignment()&CGuiObject::eAlignCenterHorizontal)
				newMaxSize.x = objWidth + centerSpacing;
			else
				newMaxSize.x = objWidth;
			if(obj->alignment()&CGuiObject::eAlignRight)
				globalPos.x += expandableWidth;
		}
		obj->render(globalPos,newMaxSize);
		if(obj->alignment()&CGuiObject::eAlignCenterHorizontal)
			newMaxSize.x = objWidth + centerSpacing;
		globalPos.x += newMaxSize.x + spacing();
		expandableWidth -= newMaxSize.x - objWidth;
		if(expandableWidth<0)
			nlwarning("expandableWidth(%f)<0",expandableWidth);
	}	
}

float CGuiHBox::_width()
{
	deque<guiSPG<CGuiObject> >::iterator it;
	float res = 0;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		res += obj->width();
	}	
	if(elements.size()>1)
		return res + (elements.size()-1) * spacing();	
	else
		return res;
}

float CGuiHBox::_height()
{
	deque<guiSPG<CGuiObject> >::iterator it;
	float res = 0;
	for(it=elements.begin();it!=elements.end();it++)
	{
		CGuiObject *obj = *it;
		if(res<obj->height())
			res = obj->height();
	}	
	return res;
}


void CGuiHBox::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiHBox",CGuiHBox::Create);
}

CGuiObject *CGuiHBox::Create()
{
	CGuiObject *res = new CGuiHBox;
	
	return res;	
}


void CGuiHBox::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiBox::init(xml,node);
}

