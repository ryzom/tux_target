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





CParticles::CParticles() : CParticlesCommon()
{
	mat = C3DTask::getInstance().createMaterial();
	LuaProxy = 0;
}


CParticles::~CParticles()
{
	if(!Mesh.empty())
	{
		C3DTask::getInstance().scene().deleteInstance(Mesh);
	}
}


void CParticles::init(const string &name, const std::string &fileName, uint8 id, const CVector &position, const CVector &scale, const CAngleAxis &rotation, bool show, bool started)
{
	CParticlesCommon::init(name, fileName, id, position, scale, rotation,show,started); 

	// Get collision faces
//	loadMesh(ShapeName, Vertices, Normals, Indices,false);
	
	ShapeName = CResourceManager::getInstance().get(fileName+".ps");

	string res = CResourceManager::getInstance().get(ShapeName);
	Mesh = C3DTask::getInstance().scene().createInstance(res);
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s'", ShapeName.c_str());
	}
	Particle.cast(Mesh);
	Particle.setTransformMode (UTransformable::RotQuat);
	Particle.setOrderingLayer(2);
	Particle.activateEmitters(started);
	if(show)
		Particle.show();
	else
		Particle.hide();
	Particle.setPos(position);

	
}

void CParticles::luaInit()
{
	if(LuaProxy)
	{
		delete LuaProxy;
		LuaProxy = 0;
	}
	if(CLevelManager::getInstance().levelPresent())
	{
		LuaProxy = new CParticlesProxy(CLevelManager::getInstance().currentLevel().luaState(),this);	
		nlinfo("CParticles::luaInit(), luaState=0x%p , proxy = 0x%p",CLevelManager::getInstance().currentLevel().luaState(),LuaProxy);
	}
	else
		nlwarning("lua init : no level loaded");
}


void CParticles::renderSelection()
{
	mat.setColor(CRGBA(255,255,255,200));
	mat.setZWrite(true);
	mat.setZFunc(UMaterial::always);
	mat.setBlend(true);
	mat.setBlendFunc(UMaterial::srcalpha,UMaterial::invsrcalpha);
	
	CMatrix matrix = mesh().getMatrix();

	
}

void CParticles::update(const NLMISC::CVector &pos, const NLMISC::CVector &rot)
{
	//TODO rot
	position(pos);
}


void CParticles::enabled(bool b)
{
	if(Enabled==b) return;

	CParticlesCommon::enabled(b);
	Particle.activateEmitters(b);

	nlinfo("%s %s",b?"show":"hide",name().c_str());
}

void CParticles::position(const NLMISC::CVector &pos) 
{
	Position = pos; _changed = true; Mesh.setPos(pos);
}
