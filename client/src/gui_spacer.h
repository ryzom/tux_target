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

#ifndef MT_GUI_SPACER_H
#define MT_GUI_SPACER_H


//
// Includes
//
#include "gui_object.h"



//
// Classes
//

class CGuiSpacer : public CGuiObject
{
public:
	CGuiSpacer();
	virtual ~CGuiSpacer();

	virtual float _width() {return 0;};
	virtual float _height() {return 0;};

	static void xmlRegister();
	static CGuiObject *create();
	
	virtual void init(CGuiXml *xml,xmlNodePtr node);
};

class CGuiSpacerManager : public NLMISC::CSingleton<CGuiSpacerManager>
{
public:
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();
	
private:
	
};


#endif
