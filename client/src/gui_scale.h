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

#ifndef MT_GUI_SCALE_H
#define MT_GUI_SCALE_H


//
// Includes
//
#include "gui_bin.h"
#include "gui_button.h"


//
// Classes
//

struct CGuiScaleEventBehaviour : public CGuiEventBehaviour
{
	CGuiScaleEventBehaviour() { }
	virtual ~CGuiScaleEventBehaviour() { }
	virtual void onChanged(float value) = 0;
};

class CGuiScale : public CGuiBin
{
public:
	CGuiScale();
	virtual ~CGuiScale();
	
	virtual float borderWidth() {return 16;};
	virtual float borderHeight() {return 16;};
	
	float percent();
	void percent(float percent);
	void ptrValue(float *ptrValue);

	guiSPG<CGuiScaleEventBehaviour> eventBehaviour;

	virtual void init(CGuiXml *xml,xmlNodePtr node);
protected:
	float _percent;
	float *_ptrValue;

	guiSPG<CGuiButton> _button;
	NLMISC::CVector _buttonPressedMousePos;
	bool _buttonPressed;
	float _buttonPressedPercent;
};

class CGuiHScale : public CGuiScale
{
public:
	CGuiHScale();
	virtual ~CGuiHScale();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual void alignment(int alignment);	
	
	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
};

class CGuiVScale : public CGuiScale
{
public:
	CGuiVScale();
	virtual ~CGuiVScale();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual void alignment(int alignment);	
	
	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
};

class CGuiScaleManager : public NLMISC::CSingleton<CGuiScaleManager>
{
public:
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();

	NL3D::UMaterial materialArrow();
	NL3D::UMaterial materialProgress();
	
	friend class CGuiScale;
protected:
	NL3D::UTextureFile	*_textureArrow;
	NL3D::UMaterial _materialArrow;
	NL3D::UTextureFile	*_textureProgress;
	NL3D::UMaterial _materialProgress;
	
};


#endif
