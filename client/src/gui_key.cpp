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
#include "gui_key.h"
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

void CGuiKeyManager::init()
{
	Texture = C3DTask::instance().driver().createTextureFile(CPath::lookup("key.tga"));
	Texture->setReleasable(false);
	nlassert(Texture);
	Material = C3DTask::instance().createMaterial();
	Material.setTexture(Texture);
	Material.setBlend(true);
	Material.setZFunc(UMaterial::always);
	Material.setDoubleSided();

	CGuiKey::xmlRegister();	
}


//
//
//

CGuiKey::CGuiKey()
{
	_text = new CGuiText();
	_text->isI18N(false);
	element(_text);
}

void CGuiKey::_render(const CVector &pos, CVector &maxSize)
{
	H_AUTO2;
	quad.color(CRGBA(255,255,255,255));
	quad.offset(CVector(0,0,0));
	quad.stretched(false);	
	quad.material(CGuiKeyManager::instance().Material);

	CGuiBin::_render(pos,maxSize);
}

float CGuiKey::borderWidth() 
{
	float w = element()->width();
	return 32-((w<25)?w:20);
}

float CGuiKey::borderHeight() 
{
	return 32-element()->height();
}

void CGuiKey::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiKey", CGuiKey::create);
}

CGuiObject *CGuiKey::create()
{
	return new CGuiKey;	
}

void CGuiKey::init(CGuiXml *xml, xmlNodePtr node)
{
	CGuiBin::init(xml,node);
	string letter = xml->doc.getStringProperty(node,"key", "");
	_text->text(ucstring::makeFromUtf8(letter));
}
