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
#include "gui_bitmap.h"
#include "gui_stretched_quad.h"
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
	

void CGuiBitmapManager::init()
{
	CGuiBitmap::xmlRegister();	
}


void CGuiBitmapManager::render()
{
	
	
}

void CGuiBitmapManager::release()
{
	
}


//
//
//
CGuiBitmap::CGuiBitmap()
{
}

CGuiBitmap::CGuiBitmap(string filename)
{
	load(filename);
}

CGuiBitmap::~CGuiBitmap()
{
	
}

float CGuiBitmap::_width()
{
	if(!_material.empty())
		return 32;
	else
		return 0;
}

float CGuiBitmap::_height()
{
	if(!_material.empty())
		return 32;
	else
		return 0;	
}

void CGuiBitmap::_render(const CVector &pos,CVector &maxSize)
{
	H_AUTO2;
	CGuiStretchedQuad quad;
	CVector globalPos = globalPosition(pos,maxSize);
	CVector expSize = expandSize(maxSize);

	quad.stretched(false);	
	quad.size(expSize);
	quad.position(globalPos);
	quad.material(material());
	quad.render();

	maxSize = expSize;	
}

UMaterial CGuiBitmap::load(const string &filename)
{
	string res;
	
	res = CPath::lookup(filename);
	_texture= C3DTask::instance().driver().createTextureFile(res);
	nlassert(_texture);
	
	_material = C3DTask::instance().createMaterial();
	_material.setTexture(_texture);
	_material.setBlend(true);
	_material.setZFunc(UMaterial::always);
	_material.setDoubleSided();
	
	return _material;
}

NL3D::UMaterial CGuiBitmap::material()
{
	return _material;
}

void CGuiBitmap::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiBitmap",CGuiBitmap::create);
}

CGuiObject *CGuiBitmap::create()
{
	CGuiObject *res = new CGuiBitmap;
	
	return res;	
}

void CGuiBitmap::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiObject::init(xml,node);
	string filename;
	bool isok;

	isok = xml->getString(node,"filename",filename);
	nlassert(isok);
	load(filename);
}
