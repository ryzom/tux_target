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
#include "gui_frame.h"
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
	

void CGuiFrameManager::init()
{
	string res;
	res = CPath::lookup("frame.tga");
	_texture = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_texture);
	/*
	_texture->setWrapS(UTexture::Clamp);
	_textureFrame->setWrapT(UTexture::Clamp);
	*/
	
	_material = C3DTask::instance().createMaterial();
	_material.setTexture(_texture);
	_material.setBlend(true);
	_material.setZFunc(UMaterial::always);
	_material.setDoubleSided();
	
	CGuiFrame::xmlRegister();	
}


void CGuiFrameManager::render()
{
	
	
}

void CGuiFrameManager::release()
{
	
}

/*
NL3D::UTextureFile	*CGuiFrameManager::texture()
{
	return _texture;
}

NL3D::UMaterial *CGuiFrameManager::material()
{
	return _material;
}
*/


//
//
//
CGuiFrame::CGuiFrame() : UpdateCallback(0)
{
	quad.material(CGuiFrameManager::instance()._material);
	//alignment( CGuiObject::eAlignLeft | CGuiObject::eAlignUp);
}

CGuiFrame::~CGuiFrame()
{
	
}

void CGuiFrame::_render(const CVector &pos,CVector &maxSize)
{
	H_AUTO2;
	//CVector globalPos = globalPosition(pos,maxSize);
	//maxSize = expandSize(maxSize);

	//CScissor scissor;
	//scissor.init(globalPos.x,1.0f-globalPos.y-maxSize.y,maxSize.x,maxSize.y);
	//C3DTask::instance().driver().setScissor(scissor);

	CGuiBin::_render(pos,maxSize);

	//scissor.initFullScreen();
	//C3DTask::instance().driver().setScissor(scissor);	
}

void CGuiFrame::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiFrame",CGuiFrame::create);
}

CGuiObject *CGuiFrame::create()
{
	CGuiObject *res = new CGuiFrame;
	
	return res;	
}

void CGuiFrame::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiBin::init(xml,node);
}

