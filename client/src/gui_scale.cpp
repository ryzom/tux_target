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
#include "gui_scale.h"
#include "gui_text.h"
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

void CGuiScaleManager::init()
{
	string res;

	res = CPath::lookup("arrow.tga");
	_textureArrow = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_textureArrow);
	
	_materialArrow = C3DTask::instance().createMaterial();
	_materialArrow.setTexture(_textureArrow);
	_materialArrow.setBlend(true);
	_materialArrow.setZFunc(UMaterial::always);
	_materialArrow.setDoubleSided();
	
	res = CPath::lookup("progress.tga");
	_textureProgress = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_textureProgress);
	
	_materialProgress = C3DTask::instance().createMaterial();
	_materialProgress.setTexture(_textureProgress);
	_materialProgress.setBlend(true);
	_materialProgress.setZFunc(UMaterial::always);
	_materialProgress.setDoubleSided();
	
	CGuiHScale::xmlRegister();
	CGuiVScale::xmlRegister();
	
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
	_percent = -1.0f; // not set
	quad.material(CGuiScaleManager::instance().materialArrow());
	_button = new CGuiButton();

	_buttonPressed = false;
	_buttonPressedPercent = 0;
	_ptrValue = 0;
	eventBehaviour = 0;
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
	if(_percent != percent)
	{
		_percent = percent;
		if(_ptrValue) *_ptrValue = percent;
		if(eventBehaviour) eventBehaviour->onChanged(percent);
	}
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
	H_AUTO2;
	
	float newPercent;

	if(_ptrValue)
 		newPercent = *_ptrValue;
	else
		newPercent = percent();

	CVector globalPos = globalPosition(pos,maxSize);
	CVector expSize = expandSize(maxSize);
	
	CVector scrollPos1((expSize.x-4-_button->width())*newPercent,0,0);
	CVector mousePos = CGuiObjectManager::instance().mouseListener().position();
	CVector mousePressedPos = CGuiObjectManager::instance().mouseListener().pressedPosition();
	
	
	if( !_buttonPressed && CGuiObjectManager::instance().mouseListener().ButtonDown &&_button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
	{
		_buttonPressed = true;
		_buttonPressedMousePos = mousePos;	
		_buttonPressedPercent = newPercent;
	}
	if(_buttonPressed)
	{
		newPercent = _buttonPressedPercent + (mousePos.x - _buttonPressedMousePos.x) / (expSize.x + -_button->width());
	}
	
	if( !_buttonPressed && CGuiObjectManager::instance().mouseListener().Clicked 
		&& isIn(mousePos,pos,maxSize) && isIn(mousePressedPos,pos,maxSize)
		&& !(_button->isIn(mousePos,pos+scrollPos1,maxSize) && _button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
		)
	{
		if( mousePos.x < (globalPos.x + expSize.x * newPercent) )
			newPercent -= 0.1f;
		else
			newPercent += 0.1f;
	}
	else if (CGuiObjectManager::instance().mouseListener().ButtonDown)
	{
		if(_button->isIn(mousePos,pos+scrollPos1,maxSize) )
		{
			
		}	
	}
	
	if(CGuiObjectManager::instance().mouseListener().Clicked)
		_buttonPressed = false;
	
	newPercent = min(1.0f, newPercent);
	newPercent = max(0.0f, newPercent);
	
	float ipos = 1.0f - newPercent;
	
	float dy = expSize.y - borderHeight();
	expSize.y = borderHeight();
	
	quad.size(expSize);
	quad.position(globalPos + CVector(0,dy/2,0));
	quad.material(CGuiScaleManager::instance().materialProgress());
	quad.render();
	
	CVector scrollPos((expSize.x-_button->width())*newPercent,0,0);
	
	_button->element(element());
	_button->render(pos+scrollPos,maxSize);
	
	maxSize = expSize;	

	percent(newPercent);
// 	if(_ptrValue)
// 		*_ptrValue = _percent;
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

void CGuiHScale::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiHScale",CGuiHScale::create);
}

CGuiObject *CGuiHScale::create()
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
	H_AUTO2;
	CVector globalPos = globalPosition(pos,maxSize);
	CVector expSize = expandSize(maxSize);
	
	CVector scrollPos1(0,(expSize.y-4-_button->height())*_percent,0);
	CVector mousePos = CGuiObjectManager::instance().mouseListener().position();
	CVector mousePressedPos = CGuiObjectManager::instance().mouseListener().pressedPosition();
	
	
	if( !_buttonPressed && CGuiObjectManager::instance().mouseListener().ButtonDown &&_button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
	{
		_buttonPressed = true;
		_buttonPressedMousePos = mousePos;	
		_buttonPressedPercent = _percent;
	}
	if(_buttonPressed)
	{
		_percent = _buttonPressedPercent + (mousePos.y - _buttonPressedMousePos.y) / (expSize.y + -_button->height());
	}
	
	if( !_buttonPressed && CGuiObjectManager::instance().mouseListener().Clicked 
		&& isIn(mousePos,pos,maxSize) && isIn(mousePressedPos,pos,maxSize)
		&& !(_button->isIn(mousePos,pos+scrollPos1,maxSize) && _button->isIn(mousePressedPos,pos+scrollPos1,maxSize))
		)
	{
		if( mousePos.y < (globalPos.y + expSize.y * _percent) )
			_percent -= 0.1f;
		else
			_percent += 0.1f;
	}
	else if (CGuiObjectManager::instance().mouseListener().ButtonDown)
	{
		if(_button->isIn(mousePos,pos+scrollPos1,maxSize) )
		{
			
		}	
	}
	
	if(CGuiObjectManager::instance().mouseListener().Clicked)
		_buttonPressed = false;
	
	_percent = min(1.0f,_percent);
	_percent = max(0.0f,_percent);
	
	float ipos = 1.0f - _percent;
	
	float dx = expSize.x - borderWidth();
	expSize.x = borderWidth();
	
	quad.size(expSize);
	quad.position(globalPos + CVector(dx/2.0f,0,0));
	quad.material(CGuiScaleManager::instance().materialProgress());
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

void CGuiVScale::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiVScale",CGuiVScale::create);
}

CGuiObject *CGuiVScale::create()
{
	CGuiObject *res = new CGuiVScale;
	
	return res;	
}

void CGuiVScale::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiScale::init(xml,node);
}

