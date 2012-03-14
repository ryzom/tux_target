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

#ifndef MTPT_GUI_SCALE_H
#define MTPT_GUI_SCALE_H


//
// Includes
//
#include "gui_bin.h"
#include "gui_button.h"


//
// Classes
//

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
	
	static void XmlRegister();
	static CGuiObject *Create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
};

class CGuiVScale : public CGuiScale
{
public:
	CGuiVScale();
	virtual ~CGuiVScale();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual void alignment(int alignment);	
	
	static void XmlRegister();
	static CGuiObject *Create();
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
