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
#include "start_point.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CStartPoint::CStartPoint() : CStartPointCommon()
{
	ShapeName = CPath::lookup("col_box.shape");

	Mesh = C3DTask::instance().scene().createInstance (ShapeName);
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s'", ShapeName.c_str());
	}
}

CStartPoint::~CStartPoint()
{
	if(!Mesh.empty())
	{
		C3DTask::instance().scene().deleteInstance(Mesh);
	}
}
