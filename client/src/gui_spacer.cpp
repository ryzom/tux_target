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
#include "gui_spacer.h"


//
// Namespaces
//

using namespace NL3D;


//
// Functions
//

void CGuiSpacerManager::init()
{
	CGuiSpacer::xmlRegister();
}

void CGuiSpacerManager::render()
{
}

void CGuiSpacerManager::release()
{
}


//
//
//
CGuiSpacer::CGuiSpacer()
{
}

CGuiSpacer::~CGuiSpacer()
{
	
}

void CGuiSpacer::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiSpacer",CGuiSpacer::create);
}

CGuiObject *CGuiSpacer::create()
{
	CGuiObject *res = new CGuiSpacer;
	
	return res;	
}


void CGuiSpacer::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiObject::init(xml,node);
}

