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

#include <deque>
#include <string>

#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/material.h>
#include <nel/3d/register_3d.h>

#include <nel/misc/quat.h>
#include <nel/misc/common.h>

#include <nel/3d/u_instance_material.h>

#include <nel/3d/water_height_map.h>
#include <nel/3d/water_pool_manager.h>

#include "global.h"
#include "entity.h"
#include "network_task.h"
#include "mtp_target.h"
#include "resource_manager2.h"
#include "level_manager.h"

#include "stdpch.h"

#include <deque>

#include <nel/misc/quat.h>
#include <nel/misc/common.h>

#include <nel/3d/u_instance_material.h>

#include <nel/3d/water_height_map.h>
#include <nel/3d/water_pool_manager.h>

#include "module.h"
#include "global.h"
#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "font_manager.h"
#include "config_file_task.h"
#include "../../common/load_mesh.h"

using namespace std;
using namespace NLMISC;
using namespace NL3D;





CModule::CModule() : CModuleCommon()
{
	mat = C3DTask::getInstance().createMaterial();
	LuaProxy = 0;
}


CModule::~CModule()
{
	if(!Mesh.empty())
	{
		C3DTask::getInstance().scene().deleteInstance(Mesh);
	}
}


void CModule::init(const string &name, const std::string &shapeName, uint8 id, const CVector &position, const CVector &scale, const CAngleAxis &rotation, const NLMISC::CRGBA &color)
{
	CModuleCommon::init(name, shapeName, id, position, scale, rotation,color); 

	// Get collision faces
//	loadMesh(ShapeName, Vertices, Normals, Indices,false);
	
	ShapeName = CResourceManager::getInstance().get(shapeName+".shape");
	NbFaces = loadMesh(ShapeName, Vertices, Normals, Indices, AutoEdges);

	Mesh = C3DTask::getInstance().scene().createInstance (ShapeName);
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s.shape'", Name.c_str());
	}
	Mesh.setTransformMode(UTransformable::RotQuat);
	Mesh.setRotQuat(CQuat(rotation));
	Mesh.setPos(position);
	CVector oldScale = Mesh.getScale();
	oldScale.x *= scale.x;
	oldScale.y *= scale.y;
	oldScale.z *= scale.z;
	Mesh.setScale(oldScale);
	CMatrix mmatrix = mesh().getMatrix();
	Color.set(255,255,255);
	this->color(color);
	
}

void CModule::luaInit()
{
	if(LuaProxy)
	{
		delete LuaProxy;
		LuaProxy = 0;
	}
	if(CLevelManager::getInstance().levelPresent())
	{
		LuaProxy = new CModuleProxy(CLevelManager::getInstance().currentLevel().luaState(),this);	
		nlinfo("CModule::luaInit(), luaState=0x%p , proxy = 0x%p",CLevelManager::getInstance().currentLevel().luaState(),LuaProxy);
	}
	else
		nlwarning("lua init : no level loaded");
	
}


void CModule::renderSelection()
{
	mat.setColor(CRGBA(255,255,255,200));
	mat.setZWrite(true);
	mat.setZFunc(UMaterial::always);
	mat.setBlend(true);
	mat.setBlendFunc(UMaterial::srcalpha,UMaterial::invsrcalpha);
	
	CMatrix matrix = mesh().getMatrix();

	float grow = 0.001f; 
	uint i;
	for(i = 0; i<NbFaces; i++)
	{
		CTriangle tri;
		tri.V0 = matrix * (Vertices[Indices[i*3+0]] + grow * Normals[Indices[i*3+0]]);
		tri.V1 = matrix * (Vertices[Indices[i*3+1]] + grow * Normals[Indices[i*3+1]]);
		tri.V2 = matrix * (Vertices[Indices[i*3+2]] + grow * Normals[Indices[i*3+2]]);

#if 0
		C3DTask::getInstance().driver().drawTriangle(tri,mat);
#else
		CLine line1(tri.V0,tri.V1);
		CLine line2(tri.V1,tri.V2);
		CLine line3(tri.V2,tri.V0);
		
		C3DTask::getInstance().driver().drawLine(line1,mat);
		C3DTask::getInstance().driver().drawLine(line2,mat);
		C3DTask::getInstance().driver().drawLine(line3,mat);
#endif
		
	}
	
}

void CModule::update(const NLMISC::CVector &pos, const NLMISC::CVector &rot)
{
	//TODO rot
	position(pos);
}


void CModule::enabled(bool b)
{
	if(Enabled==b || Mesh.empty()) return;

	CModuleCommon::enabled(b);

	if(b)
		Mesh.show();
	else
		Mesh.hide();

	nlinfo("%s %s",b?"show":"hide",name().c_str());
}

void  CModule::color(const NLMISC::CRGBA &col) 
{ 
	if(Color==col || Mesh.empty()) return;
	
	Color = col; 
	for(uint i = 0; i < Mesh.getNumMaterials(); i++)
	{
		Mesh.getMaterial(i).setDiffuse(Color);
		Mesh.getMaterial(i).setAmbient(Color);
	}
}
