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


//
// Includes
//

#include "stdpch.h"

#include "3d_task.h"
#include "time_task.h"
#include "gui_check.h"
#include "gui_xml.h"

#include <nel/3d/u_material.h>


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//
	
struct checkMaterialData
{
	checkMaterialData(const char *name) : filename(name)
	{
	}
	NL3D::UTextureFile	*texture;
	NL3D::UMaterial material;
	string filename;
};

static checkMaterialData checkMaterials[] = 
{
	checkMaterialData("hbutton-prelight.tga"),
	checkMaterialData("hbutton-insensitive.tga"),
	checkMaterialData("hbutton.tga"),
	checkMaterialData("hbutton-active.tga"),
};



static int checkMaterialsSize = sizeof(checkMaterials) / sizeof(checkMaterialData);

void CGuiCheckManager::init()
{
	string res;

	for(int i=0;i<checkMaterialsSize;i++)
	{
		res = CPath::lookup(checkMaterials[i].filename);
		checkMaterials[i].texture = C3DTask::instance().driver().createTextureFile(res);
		checkMaterials[i].texture->setReleasable(false);
		nlassert(checkMaterials[i].texture);
		checkMaterials[i].material = C3DTask::instance().createMaterial();
		checkMaterials[i].material.setTexture(checkMaterials[i].texture);
		checkMaterials[i].material.setBlend(true);
		checkMaterials[i].material.setZFunc(UMaterial::always);
		checkMaterials[i].material.setDoubleSided();
	}

	CGuiCheck::xmlRegister();
	
}
	
void CGuiCheckManager::render()
{
	
}

void CGuiCheckManager::release()
{
	
}

NL3D::UTextureFile	*CGuiCheckManager::texture(TCheckMaterialId id)
{
	return checkMaterials[id].texture;
}

NL3D::UMaterial CGuiCheckManager::material(TCheckMaterialId id)
{
	return checkMaterials[id].material;
}

//
//
//

void CGuiCheck::_init()
{
	_checked = false;
	_text = new CGuiText();
	_text->isI18N(false);
	_pressed = false;
	eventBehaviour = 0;
}

CGuiCheck::CGuiCheck()
{
	resetBitmap();
	_init();
}

CGuiCheck::CGuiCheck(UMaterial normalBitmap)
{
	resetBitmap(normalBitmap);
	_init();
}

CGuiCheck::CGuiCheck(UMaterial normalBitmap, UMaterial activeBitmap)
{
	resetBitmap(normalBitmap,activeBitmap);
	_init();
}

CGuiCheck::CGuiCheck(const string &normalBitmap)
{
	resetBitmap(normalBitmap);
	_init();
}

CGuiCheck::CGuiCheck(const string &normalBitmap, const string &activeBitmap)
{
	resetBitmap(normalBitmap,activeBitmap);
	_init();
}

void CGuiCheck::resetBitmap()
{
	_normalBitmap       = CGuiCheckManager::instance().material(CGuiCheckManager::eNormal);
	_activeBitmap       = CGuiCheckManager::instance().material(CGuiCheckManager::eActive);
	_prelightBitmap     = CGuiCheckManager::instance().material(CGuiCheckManager::ePrelight);
	_insensitiveBitmap  = CGuiCheckManager::instance().material(CGuiCheckManager::eInsensitive);
	_stretched          = true;
}

void CGuiCheck::resetBitmap(UMaterial normalBitmap)
{
	_normalBitmap       = normalBitmap;
	_activeBitmap       = 0;
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;
}

void CGuiCheck::resetBitmap(UMaterial normalBitmap, UMaterial activeBitmap)
{
	_normalBitmap       = normalBitmap;
	_activeBitmap       = activeBitmap;
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;
}

void CGuiCheck::resetBitmap(const string &normalBitmap)
{
	_normalBitmap       = CGuiObject::LoadBitmap(normalBitmap);	
	_activeBitmap       = 0;
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;
}

void CGuiCheck::resetBitmap(const string &normalBitmap, const string &activeBitmap)
{
	_normalBitmap       = CGuiObject::LoadBitmap(normalBitmap);	
	_activeBitmap       = CGuiObject::LoadBitmap(activeBitmap);	
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;	
}

CGuiCheck::~CGuiCheck()
{
}

void CGuiCheck::_render(const CVector &pos, CVector &maxSize)
{
	H_AUTO2;
	CGuiCheckManager::TCheckMaterialId checkState = CGuiCheckManager::eNormal;
	quad.color(CRGBA(255,255,255,255));
	quad.offset(CVector(0,0,0));
	quad.stretched(_stretched);	
	
	_pressed = false;
	CVector mousePos = CGuiObjectManager::instance().mouseListener().position();

	if(isIn(mousePos,pos,maxSize))
	{
		CVector mousePressedPos = CGuiObjectManager::instance().mouseListener().pressedPosition();
		
		if(CGuiObjectManager::instance().mouseListener().ButtonDown)
		{
			if(isIn(mousePressedPos,pos,maxSize))
				checkState = CGuiCheckManager::eActive;
		}
		else
			checkState = CGuiCheckManager::ePrelight;
			
		if(CGuiObjectManager::instance().mouseListener().Clicked && isIn(mousePressedPos,pos,maxSize))
		{
			_onPressed();
			_pressed = true;
		}
		
	}
	else
		checkState = CGuiCheckManager::eNormal;

	if(checkState==CGuiCheckManager::eNormal)
		quad.material(_normalBitmap);

	if(checkState==CGuiCheckManager::eActive)
	{
		if(!_activeBitmap.empty())
			quad.material(_activeBitmap);
		else
		{
			quad.material(_normalBitmap);
			quad.color(CRGBA(200,200,200,255));
			quad.offset(CVector(1,1,0));
		}		
	}
	
	if(checkState==CGuiCheckManager::ePrelight)
	{
		if(!_prelightBitmap.empty())
			quad.material(_prelightBitmap);
		else
		{
			quad.material(_normalBitmap);
			quad.color(CRGBA(240,240,255,255));
		}		
	}

	CGuiBin::_render(pos,maxSize);

	CVector pos2 = pos;
	pos2.x += 2;
	pos2.y += 2;
	_text->render(pos2,maxSize);
}

void CGuiCheck::alignment(int alignment)
{
	if(alignment&eAlignLeft)
		_text->alignment(eAlignLeft);
	if(alignment&eAlignRight)
		_text->alignment(eAlignRight);
	if(alignment&eAlignCenterHorizontal)
		_text->alignment(eAlignCenterHorizontal);
	CGuiObject::alignment(alignment);
}

void CGuiCheck::onPressed()
{
}

void CGuiCheck::_onPressed()
{
	if(_onClickScript.size())
		luaDoString(_xml->LuaState,_onClickScript.c_str());

	_checked = !_checked;

// 	CGuiText *text = new CGuiText(ucstring("X"));
// 	element(text);

 	if(_checked)
 		_text->text(ucstring("X"));
 	else
 		_text->text(ucstring(""));

	if(eventBehaviour)
		eventBehaviour->onPressed(_checked);

	onPressed();
}

bool CGuiCheck::pressed()
{
	bool res = _pressed;
	_pressed = false;
	return res;
}

float CGuiCheck::borderWidth() 
{
	if(_stretched)
		return 16;
	else
		return 0;
}

float CGuiCheck::borderHeight() 
{
	if(_stretched)
		return 16;
	else
		return 0;
};

void CGuiCheck::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiCheck",CGuiCheck::create);
}

CGuiObject *CGuiCheck::create()
{
	CGuiObject *res = new CGuiCheck;
	
	return res;	
}

void CGuiCheck::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiBin::init(xml,node);
	bool hasNormalBitmap;
	bool hasActiveBitmap;

	string normalBitmapFilename;
	hasNormalBitmap = xml->getString(node,"normalBitmapFilename",normalBitmapFilename);
	string activeBitmapFilename;
	hasActiveBitmap = xml->getString(node,"activeBitmapFilename",activeBitmapFilename);

	if(hasNormalBitmap && hasActiveBitmap)
		resetBitmap(normalBitmapFilename,activeBitmapFilename);
	else if(hasNormalBitmap)
		resetBitmap(normalBitmapFilename);
	else
		resetBitmap();
	
	xmlNodePtr snode = xml->doc.getFirstChildNode(node,"script");
	if(snode)
	{
		_onClickScript=  "";
		xml->getString(snode,"onClick",_onClickScript);
	}

}
