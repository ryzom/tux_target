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

#ifndef MT_GUI_CHECK_H
#define MT_GUI_CHECK_H


//
// Includes
//

#include "gui_bin.h"
#include "gui_text.h"


//
// Classes
//

struct CGuiCheckEventBehaviour : public CGuiEventBehaviour
{
	CGuiCheckEventBehaviour() { }
	virtual ~CGuiCheckEventBehaviour() { }
	virtual void onPressed(bool on) = 0;
};

class CGuiCheck : public CGuiBin
{
public:
	CGuiCheck(NL3D::UMaterial normalBitmap);
	CGuiCheck(NL3D::UMaterial normalBitmap, NL3D::UMaterial activeBitmap);
	CGuiCheck(const string &normalBitmap);
	CGuiCheck(const string &normalBitmap, const string &activeBitmap);
	CGuiCheck();
	virtual ~CGuiCheck();

	void resetBitmap();
	void resetBitmap(NL3D::UMaterial normalBitmap);
	void resetBitmap(NL3D::UMaterial normalBitmap, NL3D::UMaterial activeBitmap);
	void resetBitmap(const string &normalBitmap);
	void resetBitmap(const string &normalBitmap, const string &activeBitmap);
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);

	virtual void onPressed();
	
	virtual float borderWidth();
	virtual float borderHeight();

	void alignment(int alignment);

	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
	bool pressed();

	bool checked() const { return _checked; }

	void checked(bool on) {
		_checked = on;
		if(_checked)
			_text->text(ucstring("X"));
		else
			_text->text(ucstring(""));
	}

	guiSPG<CGuiCheckEventBehaviour> eventBehaviour;

	guiSPG<CGuiText> _text;

private:

	void _init();
	void _onPressed();
	
	NL3D::UMaterial _prelightBitmap;
	NL3D::UMaterial _insensitiveBitmap;
	NL3D::UMaterial _normalBitmap;
	NL3D::UMaterial _activeBitmap;

	bool _checked;

	bool _stretched;
	bool _pressed;
	string _onClickScript;
};

class CGuiCheckManager : public NLMISC::CSingleton<CGuiCheckManager>
{
public:
	enum TCheckMaterialId 
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

	NL3D::UTextureFile	*texture(TCheckMaterialId id);
	NL3D::UMaterial material(TCheckMaterialId id);
	
protected:
	
};

#endif
