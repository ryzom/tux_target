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

#ifndef MT_GUI_PROGRESS_BAR_H
#define MT_GUI_PROGRESS_BAR_H


//
// Includes
//

#include "gui_bin.h"


//
// Classes
//

class CGuiProgressBar : public CGuiBin
{
public:
	CGuiProgressBar();
	virtual ~CGuiProgressBar();

	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	
	virtual float borderWidth() {return 16;};
	virtual float borderHeight() {return 16;};

	float percent();
	void percent(float percent);
	void ptrValue(float *ptrValue);
	
	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
private:
	float _percent;
	float *_ptrValue;
};

class CGuiProgressBarManager : public NLMISC::CSingleton<CGuiProgressBarManager>
{
public:
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();

	friend class CGuiProgressBar;
protected:
	NL3D::UTextureFile	*_textureProgress;
	NL3D::UMaterial _materialProgress;
	NL3D::UTextureFile	*_textureProgressBar;
	NL3D::UMaterial _materialProgressBar;
	
};


#endif
