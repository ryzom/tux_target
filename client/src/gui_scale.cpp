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
#include "gui_scale.h"
#include "gui_text.h"
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
	

void CGuiScaleManager::init()
{
	string res;

	res = CResourceManager::getInstance().get("arrow.tga");
	_textureArrow = C3DTask::getInstance().driver().createTextureFile(res);
	nlassert(_textureArrow);
	
	_materialArrow = C3DTask::getInstance().createMaterial();
	_materialArrow.setTexture(_textureArrow);
	_materialArrow.setBlend(true);
	_materialArrow.setZFunc(UMaterial::always);
	_materialArrow.setDoubleSided();
	
	res = CResourceManager::getInstance().get("progress.tga");
	_textureProgress = C3DTask::getInstance().driver().createTextureFile(res);
	nlassert(_textureProgress);
	
	_materialProgress = C3DTask::getInstance().createMaterial();
	_materialProgress.setTexture(_textureProgress);
	_materialProgress.setBlend(true);
	_materialProgress.setZFunc(UMaterial::always);
	_materialProgress.setDoubleSided();
	
	CGuiHScale::XmlRegister();
	CGuiVScale::XmlRegister();
	
}


void CGuiScaleManager::render()
{
	
	
}

void CGuiScaleManager::release()
{
	
}

NL3D::UMaterial CGuiScaleManager::materialArrow()
{
	return _materialArrow;
}

NL3D::UMaterial CGuiScaleManager::materialProgress()
{
	return _materialProgress;
}



//
//
//
CGuiScale::CGuiScale()
{
	_percent = 0.1f;
	quad.material(CGuiScaleManager::getInstance().materialArrow());
	_button = new CGuiButton();
	
	_buttonPressed = false;
	_buttonPressedPercent = 0;
	_ptrValue = 0;
}

CGuiScale::~CGuiScale()
{
	
}



float CGuiScale::percent()
{
	return _percent;
}

void CGuiScale::percent(float percent)
{
	_percent = percent;
	if(_ptrValue)
		*_ptrValue = percent;
}

void CGuiScale::ptrValue(float *ptrValue)
{
	_ptrValue = ptrValue;
}


void CGuiScale::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiBin::init(xml,node);
}


//
//
//

CGuiHScale::CGuiHScale():CGuiScale()
{
	_button->alignment(eAlignLeft|eAlignCenterVertical);
}

CGuiHScale::~CGuiHScale()
{
	
}

void CGuiHScale::_render(const CVector &pos,CVector &maxSize)
{
	if(_ptrValue)
		_percent = *_ptrValue;

	CVector globalPos = globalPosition(pos,maxSize);
	CVector expSize = expandSize(maxSize);
	
	CVector scrollPos1((expSize.x-4-_button->width())*_percent,0,0);
	CVector mousePos = CGuiObjectManager::getInstance().mouseListener().position();
	CVector mousePressedPos = CGuiObjectManager::getInstance().mouseListener().pressedPosition();
	
	
	if( !_buttonPressed && CGuiObjectManager::getInstance().mouseListener().ButtonDown &&_button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
	{
		_buttonPressed = true;
		_buttonPressedMousePos = mousePos;	
		_buttonPressedPercent = _percent;
	}
	if(_buttonPressed)
	{
		_percent = _buttonPressedPercent + (mousePos.x - _buttonPressedMousePos.x) / (expSize.x + -_button->width());
	}
	
	if( !_buttonPressed && CGuiObjectManager::getInstance().mouseListener().Clicked 
		&& isIn(mousePos,pos,maxSize) && isIn(mousePressedPos,pos,maxSize)
		&& !(_button->isIn(mousePos,pos+scrollPos1,maxSize) && _button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
		)
	{
		if( mousePos.x < (globalPos.x + expSize.x * _percent) )
			_percent -= 0.1f;
		else
			_percent += 0.1f;
	}
	else if (CGuiObjectManager::getInstance().mouseListener().ButtonDown)
	{
		if(_button->isIn(mousePos,pos+scrollPos1,maxSize) )
		{
			
		}	
	}
	
	if(CGuiObjectManager::getInstance().mouseListener().Clicked)
		_buttonPressed = false;
	
	_percent = min(1.0f,_percent);
	_percent = max(0.0f,_percent);
	
	float ipos = 1.0f - _percent;
	
	float dy = expSize.y - borderHeight();
	expSize.y = borderHeight();
	
	quad.size(expSize);
	quad.position(globalPos + CVector(0,dy/2,0));
	quad.material(CGuiScaleManager::getInstance().materialProgress());
	quad.render();
	
	CVector scrollPos((expSize.x-_button->width())*_percent,0,0);
	
	_button->element(element());
	_button->render(pos+scrollPos,maxSize);
	
	maxSize = expSize;	

	if(_ptrValue)
		*_ptrValue = _percent;
}

void CGuiHScale::alignment(int alignment)
{
	if(alignment&eAlignUp)
		_button->alignment(eAlignUp);
	if(alignment&eAlignBottom)
		_button->alignment(eAlignBottom);
	if(alignment&eAlignCenterVertical)
		_button->alignment(eAlignCenterVertical);
	CGuiObject::alignment(alignment);
}

void CGuiHScale::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiHScale",CGuiHScale::Create);
}

CGuiObject *CGuiHScale::Create()
{
	CGuiObject *res = new CGuiHScale;
	
	return res;	
}

void CGuiHScale::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiScale::init(xml,node);
}



//
//
//

CGuiVScale::CGuiVScale():CGuiScale()
{
	_button->alignment(eAlignCenterHorizontal|eAlignUp);
}

CGuiVScale::~CGuiVScale()
{
	
}

void CGuiVScale::_render(const CVector &pos,CVector &maxSize)
{
	CVector globalPos = globalPosition(pos,maxSize);
	CVector expSize = expandSize(maxSize);
	
	CVector scrollPos1(0,(expSize.y-4-_button->height())*_percent,0);
	CVector mousePos = CGuiObjectManager::getInstance().mouseListener().position();
	CVector mousePressedPos = CGuiObjectManager::getInstance().mouseListener().pressedPosition();
	
	
	if( !_buttonPressed && CGuiObjectManager::getInstance().mouseListener().ButtonDown &&_button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
	{
		_buttonPressed = true;
		_buttonPressedMousePos = mousePos;	
		_buttonPressedPercent = _percent;
	}
	if(_buttonPressed)
	{
		_percent = _buttonPressedPercent + (mousePos.y - _buttonPressedMousePos.y) / (expSize.y + -_button->height());
	}
	
	if( !_buttonPressed && CGuiObjectManager::getInstance().mouseListener().Clicked 
		&& isIn(mousePos,pos,maxSize) && isIn(mousePressedPos,pos,maxSize)
		&& !(_button->isIn(mousePos,pos+scrollPos1,maxSize) && _button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
		)
	{
		if( mousePos.y < (globalPos.y + expSize.y * _percent) )
			_percent -= 0.1f;
		else
			_percent += 0.1f;
	}
	else if (CGuiObjectManager::getInstance().mouseListener().ButtonDown)
	{
		if(_button->isIn(mousePos,pos+scrollPos1,maxSize) )
		{
			
		}	
	}
	
	if(CGuiObjectManager::getInstance().mouseListener().Clicked)
		_buttonPressed = false;
	
	_percent = min(1.0f,_percent);
	_percent = max(0.0f,_percent);
	
	float ipos = 1.0f - _percent;
	
	float dx = expSize.x - borderWidth();
	expSize.x = borderWidth();
	
	quad.size(expSize);
	quad.position(globalPos + CVector(dx/2.0f,0,0));
	quad.material(CGuiScaleManager::getInstance().materialProgress());
	quad.render();
	
	CVector scrollPos(0,(expSize.y-_button->height())*_percent,0);
	
	_button->element(element());
	_button->render(pos+scrollPos,maxSize);
	
	maxSize = expSize;	
	
}

void CGuiVScale::alignment(int alignment)
{
	if(alignment&eAlignLeft)
		_button->alignment(eAlignLeft);
	if(alignment&eAlignRight)
		_button->alignment(eAlignRight);
	if(alignment&eAlignCenterHorizontal)
		_button->alignment(eAlignCenterHorizontal);
	CGuiObject::alignment(alignment);
}

void CGuiVScale::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiVScale",CGuiVScale::Create);
}

CGuiObject *CGuiVScale::Create()
{
	CGuiObject *res = new CGuiVScale;
	
	return res;	
}

void CGuiVScale::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiScale::init(xml,node);
}

