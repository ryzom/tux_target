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

#ifndef MT_GUI_BOX_H
#define MT_GUI_BOX_H


//
// Includes
//

#include <nel/misc/singleton.h>

#include <nel/3d/u_material.h>

#include "gui_stretched_quad.h"
#include "gui_container.h"


//
// Classes
//

class CGuiBox : public CGuiContainer
{
public:
	
	CGuiBox();
	virtual ~CGuiBox();

	virtual CGuiObject::TGuiAlignment alignment();
	virtual void alignment(int alignment);
	
	float spacing();
	void spacing(float spacing);
	
	virtual void init(CGuiXml *xml,xmlNodePtr node);

	deque<guiSPG<CGuiObject> > elements;		

	virtual void luaPush(lua_State *L);

	LUA_BEGIN(CGuiBox)
		bind_method(CGuiBox, getName),	
		bind_method(CGuiBox, getCount),	
		bind_method(CGuiBox, getElement),	
	LUA_END


	int getName(lua_State *luaSession);
	int getCount(lua_State *luaSession);
	int getElement(lua_State *luaSession);
private:	
	float _spacing;
	CGuiStretchedQuad quad;
};

class CGuiVBox : public CGuiBox
{
public:
	
	CGuiVBox();
	virtual ~CGuiVBox();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);	
	
	virtual float _width();
	virtual float _height();

	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);

private:
	
};

class CGuiHBox : public CGuiBox
{
public:
	
	CGuiHBox();
	virtual ~CGuiHBox();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);	
	
	virtual float _width();
	virtual float _height();

	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
private:
	
};

class CGuiBoxManager : public NLMISC::CSingleton<CGuiBoxManager>
{
public:
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();
	
	NL3D::UTextureFile	*texture();
	NL3D::UMaterial material();

private:
	NL3D::UTextureFile	*_texture;
	NL3D::UMaterial _material;
};

#endif
