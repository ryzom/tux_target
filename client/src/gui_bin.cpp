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
#include "gui_bin.h"
#include "gui_xml.h"



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
	

void CGuiBinManager::init()
{
}
	
void CGuiBinManager::render()
{
}

void CGuiBinManager::release()
{
	
}


//
//
//
CGuiBin::CGuiBin()
{
	_element = 0;
}

CGuiBin::~CGuiBin()
{
	_element = 0;
}

void CGuiBin::_render(const CVector &pos,CVector &maxSize)
{
	CVector size = maxSize;
	renderQuad(pos,maxSize);
	renderElement(pos,size);
}	

void CGuiBin::renderElement(const CVector &pos,CVector &maxSize)
{
	CVector expSize = expandSize(maxSize);
	CVector globalPos = globalPosition(pos,maxSize);
	CVector elementSize = CVectorMax(innerSize(),expSize-borderSize());
	maxSize = elementSize + borderSize();
	
	if(element())
		element()->render(globalPos+borderSize()/2,elementSize);	
}	


void CGuiBin::renderQuad(const CVector &pos,CVector &maxSize)
{
	CVector expSize = expandSize(maxSize);
	CVector globalPos = globalPosition(pos,maxSize);
	CVector elementSize = CVectorMax(innerSize(),expSize-borderSize());
	maxSize = elementSize + borderSize();
	
	quad.size(maxSize);
	quad.position(globalPos);
	quad.render();
}	



CGuiObject *CGuiBin::element()
{
	return _element;
}

void CGuiBin::element(CGuiObject *element)
{
	_element = element;	
}


float CGuiBin::_width()
{
	return innerWidth() + borderWidth();
}

float CGuiBin::_height()
{
	return innerHeight() + borderHeight();
}

float CGuiBin::innerWidth()
{
	float res;
	if(_element)
		res = _element->width();
	else
		res = 0;
	return res;	
}

float CGuiBin::innerHeight()
{
	float res;
	if(_element)
		res = _element->height();
	else
		res = 0;
	return res;	
}

CVector CGuiBin::innerSize()
{
	return CVector(innerWidth(),innerHeight(),0);
}

CVector CGuiBin::borderSize()
{
	return CVector(borderWidth(),borderHeight(),0);
}

void CGuiBin::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiContainer::init(xml,node);

	node = xml->doc.getFirstChildNode(node,"element");
	if(node)
	{
		CGuiObject *object = 0;
		for( node = xml->doc.getFirstChildNode(node,"object");node;node = xml->doc.getNextChildNode(node,"object") )
		{
			object = CGuiObject::XmlCreateFromNode(xml,node);
			element(object);
		}
		if(object)
			xml->objects.push_back(object);
	}
}

