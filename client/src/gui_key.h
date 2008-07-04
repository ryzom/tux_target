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

#ifndef MT_GUI_KEY_H
#define MT_GUI_KEY_H


//
// Includes
//

#include "gui_bin.h"
#include "gui_text.h"


//
// Classes
//

class CGuiKey : public CGuiBin
{
public:
	CGuiKey();
	virtual ~CGuiKey() {}

	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);

	virtual float borderWidth();
	virtual float borderHeight();

	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);

private:
	guiSPG<CGuiText> _text;
};

class CGuiKeyManager : public NLMISC::CSingleton<CGuiKeyManager>
{
public:
	virtual void init();
	virtual void update() {}
	virtual void render() {}
	virtual void release() {}

	NL3D::UTextureFile	*Texture;
	NL3D::UMaterial Material;
};

#endif
