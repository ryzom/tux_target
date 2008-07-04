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

#ifndef MTPT_GUI_SCROLLBAR_H
#define MTPT_GUI_SCROLLBAR_H

#if 0

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

private:
	float _percent;
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

#endif // 0

#endif
