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
// This is the main class that manages all other classes
//

#ifndef MTPT_GUI_BIN_H
#define MTPT_GUI_BIN_H


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
