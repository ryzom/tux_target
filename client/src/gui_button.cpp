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
// Includes
//

#include "stdpch.h"

#include "3d_task.h"
#include "time_task.h"
#include "resource_manager2.h"
#include "gui_button.h"
#include "gui_xml.h"


#include <nel/3d/u_material.h>
//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	
struct buttonMaterialData
{
	buttonMaterialData(const char *name) : filename(name)
	{
	}
	NL3D::UTextureFile	*texture;
	NL3D::UMaterial material;
	std::string filename;
};

static buttonMaterialData buttonMaterials[] = 
{
	buttonMaterialData("hbutton-prelight.tga"),
	buttonMaterialData("hbutton-insensitive.tga"),
        buttonMaterialData("hbutton.tga"),
        buttonMaterialData("hbutton-active.tga"),
};



static int buttonMaterialsSize = sizeof(buttonMaterials) / sizeof(buttonMaterialData);

void CGuiButtonManager::init()
{
	string res;

	for(int i=0;i<buttonMaterialsSize;i++)
	{
		res = CResourceManager::getInstance().get(buttonMaterials[i].filename);
		buttonMaterials[i].texture = C3DTask::getInstance().driver().createTextureFile(res);
		buttonMaterials[i].texture->setReleasable(false);
		nlassert(buttonMaterials[i].texture);
		buttonMaterials[i].material = C3DTask::getInstance().createMaterial();
		buttonMaterials[i].material.setTexture(buttonMaterials[i].texture);
		buttonMaterials[i].material.setBlend(true);
		buttonMaterials[i].material.setZFunc(UMaterial::always);
		buttonMaterials[i].material.setDoubleSided();
	}

	CGuiButton::XmlRegister();
	
}
	
void CGuiButtonManager::render()
{
	
}

void CGuiButtonManager::release()
{
	
}

NL3D::UTextureFile	*CGuiButtonManager::texture(TButtonMaterialId id)
{
	return buttonMaterials[id].texture;
}

NL3D::UMaterial CGuiButtonManager::material(TButtonMaterialId id)
{
	return buttonMaterials[id].material;
}



//
//
//
void CGuiButton::_init()
{
	_pressed = false;
	eventBehaviour = 0;
}

CGuiButton::CGuiButton()
{
	resetBitmap();
	_init();
}

CGuiButton::CGuiButton(UMaterial normalBitmap)
{
	resetBitmap(normalBitmap);
	_init();
}

CGuiButton::CGuiButton(UMaterial normalBitmap, UMaterial activeBitmap)
{
	resetBitmap(normalBitmap,activeBitmap);
	_init();
}

CGuiButton::CGuiButton(const string &normalBitmap)
{
	resetBitmap(normalBitmap);
	_init();
}

CGuiButton::CGuiButton(const string &normalBitmap, const string &activeBitmap)
{
	resetBitmap(normalBitmap,activeBitmap);
	_init();
}

void CGuiButton::resetBitmap()
{
	_normalBitmap       = CGuiButtonManager::getInstance().material(CGuiButtonManager::eNormal);
	_activeBitmap       = CGuiButtonManager::getInstance().material(CGuiButtonManager::eActive);
	_prelightBitmap     = CGuiButtonManager::getInstance().material(CGuiButtonManager::ePrelight);
	_insensitiveBitmap  = CGuiButtonManager::getInstance().material(CGuiButtonManager::eInsensitive);
	_stretched          = true;
}

void CGuiButton::resetBitmap(UMaterial normalBitmap)
{
	_normalBitmap       = normalBitmap;
	_activeBitmap       = 0;
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;
}

void CGuiButton::resetBitmap(UMaterial normalBitmap, UMaterial activeBitmap)
{
	_normalBitmap       = normalBitmap;
	_activeBitmap       = activeBitmap;
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;
}

void CGuiButton::resetBitmap(const string &normalBitmap)
{
	_normalBitmap       = CGuiObject::LoadBitmap(normalBitmap);	
	_activeBitmap       = 0;
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;
}

void CGuiButton::resetBitmap(const string &normalBitmap, const string &activeBitmap)
{
	_normalBitmap       = CGuiObject::LoadBitmap(normalBitmap);	
	_activeBitmap       = CGuiObject::LoadBitmap(activeBitmap);	
	_prelightBitmap     = 0;
	_insensitiveBitmap  = 0;
	_stretched          = false;	
}

CGuiButton::~CGuiButton()
{
	
}

void CGuiButton::_render(const CVector &pos, CVector &maxSize)
{
	CGuiButtonManager::TButtonMaterialId buttonState = CGuiButtonManager::eNormal;
	quad.color(CRGBA(255,255,255,255));
	quad.offset(CVector(0,0,0));
	quad.stretched(_stretched);	
	
	_pressed = false;
	CVector mousePos = CGuiObjectManager::getInstance().mouseListener().position();

	if(isIn(mousePos,pos,maxSize))
	{
		CVector mousePressedPos = CGuiObjectManager::getInstance().mouseListener().pressedPosition();
		
		if(CGuiObjectManager::getInstance().mouseListener().ButtonDown)
		{
			if(isIn(mousePressedPos,pos,maxSize))
				buttonState = CGuiButtonManager::eActive;
		}
		else
			buttonState = CGuiButtonManager::ePrelight;
			
		if(CGuiObjectManager::getInstance().mouseListener().Clicked && isIn(mousePressedPos,pos,maxSize))
		{
			_onPressed();
			_pressed = true;
		}
		
	}
	else
		buttonState = CGuiButtonManager::eNormal;
	

	if(buttonState==CGuiButtonManager::eNormal)
		quad.material(_normalBitmap);

	if(buttonState==CGuiButtonManager::eActive)
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
	
	if(buttonState==CGuiButtonManager::ePrelight)
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
}

void CGuiButton::onPressed()
{
}

void CGuiButton::_onPressed()
{
	if(_onClickScript.size())
		lua_dostring(_xml->LuaState,_onClickScript.c_str());
	if(eventBehaviour)
		eventBehaviour->onPressed();
	onPressed();
}

bool CGuiButton::pressed()
{
	bool res = _pressed;
	_pressed = false;
	return res;
}

float CGuiButton::borderWidth() 
{
	if(_stretched)
		return 16;
	else
		return 0;
}

float CGuiButton::borderHeight() 
{
	if(_stretched)
		return 16;
	else
		return 0;
};

void CGuiButton::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiButton",CGuiButton::Create);
}

CGuiObject *CGuiButton::Create()
{
	CGuiObject *res = new CGuiButton;
	
	return res;	
}

void CGuiButton::init(CGuiXml *xml,xmlNodePtr node)
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

//
//
//


CGuiButtonEventBehaviour::CGuiButtonEventBehaviour()
{

}

CGuiButtonEventBehaviour::~CGuiButtonEventBehaviour()
{

}

void CGuiButtonEventBehaviour::onPressed()
{

}

