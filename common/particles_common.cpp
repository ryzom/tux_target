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

#include <nel/misc/path.h>

#include "particles_common.h"
#include "lua_utility.h"
#include "load_mesh.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Types
//


//
// Declarations
//


//
// Variables
//


//
// Functions
//

/*
void CParticlesCommon::init()
{
	Bounce = true;
	Score = 0;
	Accel = 0.0001f;
	Friction = 5.0f;

	_type = Module;
}
*/

CParticlesCommon::CParticlesCommon() : CEditableElementCommon()
{ 
	_type = Particles;
	Enabled = true;
}

CParticlesCommon::~CParticlesCommon()
{
}

void CParticlesCommon::init(const string &name, const std::string &fileName, uint8 id, CVector position, CVector scale, CAngleAxis rotation, bool show, bool started)
{
	LuaShow = show;
	LuaStarted = started;
	CEditableElementCommon::init(name,fileName,id,position,scale,rotation);
	nlinfo("Adding CParticlesCommon '%s'(%s) at position %f %f %f , scale : %f %f %f , show=%d, started=%d", name.c_str(), fileName.c_str(), position.x, position.y, position.z, scale.x, scale.y, scale.z,show,started);
	ShapeName = fileName+".ps";
	LuaShapeName = fileName;
}


void CParticlesCommon::display(CLog *log) const
{
}


void CParticlesCommon::enabled(bool e) 
{
	if(Enabled==e) return;
	Enabled = e; 
}


string CParticlesCommon::toLuaString()
{
	return toString("{ Position = CVector(%f,%f,%f), Scale = CVector(%f, %f, %f), Rotation = CAngleAxis(%f,%f,%f,%f), Show = %d, Started = %d, Lua=\"%s\", Shape=\"%s\" }",Position.x,Position.y,Position.z,Scale.x,Scale.y,Scale.z,Rotation.Axis.x,Rotation.Axis.y,Rotation.Axis.z,Rotation.Angle,LuaShow,LuaStarted,Name.c_str(),LuaShapeName.c_str());
}


//
//
//
