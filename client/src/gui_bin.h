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

#ifndef MT_GUI_BIN_H
#define MT_GUI_BIN_H


//
// Includes
//
#include "gui_stretched_quad.h"
#include "gui_container.h"



//
// Classes
//

class CGuiBin : public CGuiContainer
{
public:
	CGuiBin();
	virtual ~CGuiBin();
	
	virtual void _render(const NLMISC::CVector &pos,NLMISC::CVector &maxSize);
	virtual void renderQuad(const NLMISC::CVector &pos,NLMISC::CVector &maxSize);
	virtual void renderElement(const NLMISC::CVector &pos,NLMISC::CVector &maxSize);
	
	virtual NLMISC::CVector innerSize();
	virtual NLMISC::CVector borderSize();
	virtual float innerWidth();
	virtual float innerHeight();
	virtual float _width();
	virtual float _height();
	virtual float borderWidth() {return 0;};
	virtual float borderHeight() {return 0;};
	

	CGuiObject *element();
	void element(CGuiObject *element);

	virtual void init(CGuiXml *xml,xmlNodePtr node);
	
protected:
	CGuiStretchedQuad quad;
private:
	guiSPG<CGuiObject> _element;
};

class CGuiBinManager : public NLMISC::CSingleton<CGuiBinManager>
{
public:
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();
	
private:
	
};

#endif
