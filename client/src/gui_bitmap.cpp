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
#include "gui_bitmap.h"
#include "gui_stretched_quad.h"
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
	

void CGuiBitmapManager::init()
{
	CGuiBitmap::XmlRegister();	
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
	
	res = CResourceManager::getInstance().get(filename);
	_texture= C3DTask::getInstance().driver().createTextureFile(res);
	nlassert(_texture);
	
	_material = C3DTask::getInstance().createMaterial();
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

void CGuiBitmap::XmlRegister()
{
	CGuiObjectManager::getInstance().registerClass("CGuiBitmap",CGuiBitmap::Create);
}

CGuiObject *CGuiBitmap::Create()
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

