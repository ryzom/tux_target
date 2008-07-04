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

#include <nel/misc/path.h>

#include "module_common.h"
#include "lua_utility.h"
#include "load_mesh.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//

CModuleCommon::CModuleCommon() : CEditableElementCommon()
{
	Type = Module;
	Enabled = true;
}

CModuleCommon::~CModuleCommon()
{
}

/*void CModuleCommon::init(const string &name, const string &shapeName, uint8 id, CVector position, CVector scale, CAngleAxis rotation, const NLMISC::CRGBA &color)
{
	CEditableElementCommon::init(name,shapeName,id,position,scale,rotation);
	nlinfo("Adding module '%s'(%s) at position %f %f %f , scale : %f %f %f, color = %d %d %d %d", name.c_str(), shapeName.c_str(), position.x, position.y, position.z, scale.x, scale.y, scale.z,color.R,color.G,color.B,color.A);
	ShapeName = shapeName;
	LuaShapeName = shapeName;
}*/

void CModuleCommon::display(CLog *log) const
{
	const CLuaVector &pos = position();
	log->displayNL("  name '%s' pos (%.2g, %.2g, %.2g) col: %d vertices %d faces", name().c_str(), pos.x, pos.y, pos.z, Vertices.size()/3, Indices.size()/3);
	log->displayNL("  score %d accel %g friction %g %sbounce", score(), accel(), friction(), (bounce()?"":"no "));
}
