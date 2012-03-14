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

#ifndef MTPT_GUI_BUTTON_H
#define MTPT_GUI_BUTTON_H


//
// Includes
//
#include "gui_bin.h"



//
// Classes
//
class CGuiButtonEventBehaviour : public CGuiEventBehaviour
{
public:
	CGuiButtonEventBehaviour();
	virtual ~CGuiButtonEventBehaviour();

	virtual void onPressed();
};


class CGuiButton : public CGuiBin
{
public:
	CGuiButton(NL3D::UMaterial normalBitmap);
	CGuiButton(NL3D::UMaterial normalBitmap, NL3D::UMaterial activeBitmap);
	CGuiButton(const std::string &normalBitmap);
	CGuiButton(const std::string &normalBitmap, const std::string &activeBitmap);
	CGuiButton();
	virtual ~CGuiButton();

	void resetBitmap();
	void resetBitmap(NL3D::UMaterial normalBitmap);
	void resetBitmap(NL3D::UMaterial normalBitmap, NL3D::UMaterial activeBitmap);
	void resetBitmap(const std::string &normalBitmap);
	void resetBitmap(const std::string &normalBitmap, const std::string &activeBitmap);
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);

	virtual void onPressed();
	
	virtual float borderWidth();
	virtual float borderHeight();

	static void XmlRegister();
	static CGuiObject *Create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
	bool pressed();

	guiSPG<CGuiButtonEventBehaviour> eventBehaviour;
private:

	void _init();
	void _onPressed();
	
	NL3D::UMaterial _prelightBitmap;
	NL3D::UMaterial _insensitiveBitmap;
	NL3D::UMaterial _normalBitmap;
	NL3D::UMaterial _activeBitmap;

	bool _stretched;
	bool _pressed;
	std::string _onClickScript;
};

class CGuiButtonManager : public NLMISC::CSingleton<CGuiButtonManager>
{
public:
	enum TButtonMaterialId 
	{
		ePrelight   = 0,
		eInsensitive,
		eNormal,
		eActive,
	};
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();

	NL3D::UTextureFile	*texture(TButtonMaterialId id);
	NL3D::UMaterial material(TButtonMaterialId id);
	
protected:
	
};

#endif
