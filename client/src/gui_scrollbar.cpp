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

#if 0

#include <nel/3d/u_material.h>

#include "3d_task.h"
#include "time_task.h"
#include "gui_progress_bar.h"
#include "gui_text.h"


//
// Namespaces
//

using namespace NL3D;


//
// Functions
//

void CGuiProgressBarManager::init()
{
	string res;

	res = CPath::lookup("progress.tga");
	_textureProgress = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_textureProgress);
	
	_materialProgress = C3DTask::instance().createMaterial();
	_materialProgress->setTexture(_textureProgress);
	_materialProgress->setBlend(true);
	_materialProgress->setZFunc(UMaterial::always);
	_materialProgress->setDoubleSided();
	
	res = CPath::lookup("progress-bar.tga");
	_textureProgressBar = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_textureProgress);
	
	_materialProgressBar = C3DTask::instance().createMaterial();
	_materialProgressBar->setTexture(_textureProgressBar);
	_materialProgressBar->setBlend(true);
	_materialProgressBar->setZFunc(UMaterial::always);
	_materialProgressBar->setDoubleSided();
	
}


void CGuiProgressBarManager::render()
{
	
	
}

void CGuiProgressBarManager::release()
{
	
}


//
//
//
CGuiProgressBar::CGuiProgressBar()
{
	_percent = 0.1f;
	quad.material(CGuiProgressBarManager::instance()._materialProgress);
	CGuiTextPercent *text = new CGuiTextPercent;
	text->ptrValue(&_percent);
	element(text);
}

CGuiProgressBar::~CGuiProgressBar()
{
	
}

void CGuiProgressBar::_render(CVector pos,CVector &maxSize)
{
	CVector globalPos = globalPosition(pos,maxSize);
	CVector expSize = expandSize(maxSize);

	_percent = min(1.0f,_percent);
	_percent = max(0.0f,_percent);
	
	float ipos = 1.0f - _percent;
	
	quad.size(expSize);
	quad.position(globalPos);
	quad.material(CGuiProgressBarManager::instance()._materialProgress);
	quad.render();

	//TODO a scale quad , and rename CGuiQuad to CGuiStretchQuad
	if(_percent>0.0f)
	{
		float barWidth  = (expSize.x-4)*_percent;
		float barHeight = expSize.y-4;
		quad.size(CVector(barWidth,barHeight,0));
		quad.position(globalPos+CVector(2,2,0));
		quad.material(CGuiProgressBarManager::instance()._materialProgressBar);
		quad.render();
	}

	CGuiBin::renderElement(pos,maxSize);
	maxSize = expSize;	
}


void CGuiProgressBar::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiScale::init(node);
}

#endif // 0
