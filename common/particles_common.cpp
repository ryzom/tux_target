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

#include "particles_common.h"
#include "lua_utility.h"
#include "load_mesh.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//

CParticlesCommon::CParticlesCommon() : CEditableElementCommon()
{ 
	Type = Particles;
	Enabled = true;
}

CParticlesCommon::~CParticlesCommon()
{
}

/*void CParticlesCommon::init(const string &name, const string &fileName, uint8 id, CVector position, CVector scale, CAngleAxis rotation, bool show, bool started)
{
	LuaShow = show;
	LuaStarted = started;
	CEditableElementCommon::init(name,fileName,id,position,scale,rotation);
	nlinfo("Adding CParticlesCommon '%s'(%s) at position %f %f %f , scale : %f %f %f , show=%d, started=%d", name.c_str(), fileName.c_str(), position.x, position.y, position.z, scale.x, scale.y, scale.z,show,started);
	ShapeName = fileName+".ps";
	LuaShapeName = fileName;
}*/

void CParticlesCommon::display(CLog *log) const
{
}

void CParticlesCommon::enabled(bool e) 
{
	if(Enabled==e) return;
	Enabled = e; 
}
