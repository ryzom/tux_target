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
#include "gui_frame.h"
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
	

void CGuiFrameManager::init()
{
	string res;
	res = CResourceManager::getInstance().get("frame.tga");
	_texture = C3DTask::getInstance().driver().createTextureFile(res);
	nlassert(_texture);
	/*
	_texture->setWrapS(UTexture::Clamp);
	_textureFrame->setWrapT(UTexture::Clamp);
	*/
	
	_material = C3DTask::getInstance().createMaterial();
	_material.setTexture(_texture);
	_material.setBlend(true);
	_material.setZFunc(UMaterial::always);
	_material.setDoubleSided();
	
	CGuiFrame::XmlRegister();	
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
CGuiFrame::CGuiFrame()
{
	quad.material(CGuiFrameManager::getInstance()._material);
	//alignment( CGuiObject::eAlignLeft | CGuiObject::eAlignUp);
}

CGuiFrame::~CGuiFrame()
{
	
}

void CGuiFrame::_render(const CVector &pos,CVector &maxSize)
{
	CVector globalPos = globalPosition(pos,maxSize);
	maxSize = expandSize(maxSize);
	

	CScissor scissor;
	scissor.init(globalPos.x,1.0f-globalPos.y-maxSize.y,maxSize.x,maxSize.y);
	//C3DTask::getInstance().driver().setScissor(scissor);

	CGuiBin::_render(globalPos,maxSize);
	
	scissor.initFullScreen();
	C3DTask::getInstance().driver().setScissor(scissor);	
}


void CGuiFrame::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiFrame",CGuiFrame::Create);
}

CGuiObject *CGuiFrame::Create()
{
	CGuiObject *res = new CGuiFrame;
	
	return res;	
}

void CGuiFrame::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiBin::init(xml,node);
}

