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

#include "module_common.h"
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
void CModuleCommon::init()
{
	Bounce = true;
	Score = 0;
	Accel = 0.0001f;
	Friction = 5.0f;

	_type = Module;
}
*/

CModuleCommon::CModuleCommon() : CEditableElementCommon()
{ 
	_type = Module;
	Bounce = false;
	Collide = true;
	Score = 0;
	Accel = 0;
	Friction = 0;
	Enabled = true;
}

CModuleCommon::~CModuleCommon()
{
}

void CModuleCommon::init(const string &name, const std::string &shapeName, uint8 id, CVector position, CVector scale, CAngleAxis rotation, const NLMISC::CRGBA &color)
{
	CEditableElementCommon::init(name,shapeName,id,position,scale,rotation);
	nlinfo("Adding module '%s'(%s) at position %f %f %f , scale : %f %f %f, color = %d %d %d %d", name.c_str(), shapeName.c_str(), position.x, position.y, position.z, scale.x, scale.y, scale.z,color.R,color.G,color.B,color.A);
	ShapeName = shapeName+".shape";
	LuaShapeName = shapeName;
	LuaColor = color;
}


void CModuleCommon::display(CLog *log) const
{
	CVector pos = position();
	log->displayNL("  name '%s' pos (%.2g, %.2g, %.2g) col: %d vertices %d faces", name().c_str(), pos.x, pos.y, pos.z, Vertices.size()/3, Indices.size()/3);
	log->displayNL("  score %d accel %g friction %g %sbounce", score(), accel(), friction(), (bounce()?"":"no "));
}


void CModuleCommon::bounce(bool b) 
{
	Bounce = b; 
}

void CModuleCommon::collide(bool c) 
{
	Collide = c; 
}

void CModuleCommon::enabled(bool e) 
{
	if(Enabled==e) return;
	Enabled = e; 
}


string CModuleCommon::toLuaString()
{
	return toString("{ Position = CVector(%f,%f,%f), Scale = CVector(%f, %f, %f), Rotation = CAngleAxis(%f,%f,%f,%f), Color = CRGBA(%d,%d,%d,%d), Lua=\"%s\", Shape=\"%s\" }",Position.x,Position.y,Position.z,Scale.x,Scale.y,Scale.z,Rotation.Axis.x,Rotation.Axis.y,Rotation.Axis.z,Rotation.Angle,LuaColor.R,LuaColor.G,LuaColor.B,LuaColor.A,Name.c_str(),LuaShapeName.c_str());
}


//
//
//
