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

#include <nel/3d/u_scene.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_cloud_scape.h>
#include <nel/3d/u_text_context.h>

#include <nel/3d/scene_user.h>
#include <nel/3d/water_shape.h>
#include <nel/3d/water_model.h>
#include <nel/3d/texture_file.h>

#include "3d_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "water_task.h"
#include "score_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "entity_manager.h"
#include "background_task.h"
#include "config_file_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//


CWaterTask::CWaterTask() : ITask(), 
	WaterShape(0), WaterModel(0), WaterMesh(0), WaterInstance(0)
{
}

void cbVar (CConfigFile::CVar &var)
{
	if (!CWaterTask::instance().WaterShape) return;

	if (var.Name == "Map1Scale")
		CWaterTask::instance().WaterShape->setHeightMapScale(0, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "Map2Scale")
		CWaterTask::instance().WaterShape->setHeightMapScale(1, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "Map1Speed")
		CWaterTask::instance().WaterShape->setHeightMapSpeed(0, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "Map2Speed")
		CWaterTask::instance().WaterShape->setHeightMapSpeed(1, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else
		nlstop;
}

void CWaterTask::setWater(const string &name)
{
	if(WaterShape)
	{
		WaterShape->setEnvMap(0, new CTextureFile(CPath::lookup("water_"+name+"_env.dds")));
		WaterShape->setEnvMap(1, new CTextureFile(CPath::lookup("water_"+name+"_env.dds")));
		WaterShape->setHeightMap(0, new CTextureFile(CPath::lookup("water_"+name+"_disp.dds")));
		WaterShape->setHeightMap(1, new CTextureFile(CPath::lookup("water_"+name+"_disp2.dds")));
	}

	if(!WaterMesh.empty())
	{
		for(uint i = 0; i < WaterMesh.getNumMaterials(); i++)
		{
			WaterMesh.getMaterial(i).setTextureFileName("water_"+name+"_light.dds");
		}
	}
}

void CWaterTask::init()
{
	if(CConfigFileTask::instance().configFile().getVar("DisplayWater").asInt() == 2)
	{
		string res;
		static string shapeName("water_quad.shape");
		CSceneUser *su = dynamic_cast<CSceneUser *>(&C3DTask::instance().scene());
		CScene		&scene = su->getScene ();

		WaterShape = new CWaterShape;

		WaterShape->setWaterPoolID(0);

		CConfigFileTask::instance().configFile().setCallback("Map1Scale", cbVar);
		cbVar (CConfigFileTask::instance().configFile().getVar("Map1Scale"));
		CConfigFileTask::instance().configFile().setCallback("Map2Scale", cbVar);
		cbVar (CConfigFileTask::instance().configFile().getVar("Map2Scale"));
		CConfigFileTask::instance().configFile().setCallback("Map1Speed", cbVar);
		cbVar (CConfigFileTask::instance().configFile().getVar("Map1Speed"));
		CConfigFileTask::instance().configFile().setCallback("Map2Speed", cbVar);
		cbVar (CConfigFileTask::instance().configFile().getVar("Map2Speed"));

		const float wqSize = 10000.0f;
		CVector2f corners[] =	{ CVector2f(-wqSize, wqSize), CVector2f(wqSize, wqSize),
								  CVector2f(wqSize, -wqSize), CVector2f(-wqSize, -wqSize)
								};
		CPolygon2D waterPoly;
		waterPoly.Vertices.resize(4);
		copy(corners, corners + 4, waterPoly.Vertices.begin());
		WaterShape->setShape(waterPoly);

		scene.getShapeBank()->add(shapeName, WaterShape);

		WaterInstance = scene.createInstance(shapeName);
		WaterModel = NLMISC::safe_cast<CWaterModel *>(WaterInstance);
		WaterModel->setPos(0.0f,0.0f,1.0f*GScale);
	}
	else if(CConfigFileTask::instance().configFile().getVar("DisplayWater").asInt() == 1)
	{
		string res = CPath::lookup("water_light.shape");
		WaterMesh = C3DTask::instance().scene().createInstance(res);
		WaterMesh.setPos(0.0f,0.0f,1.0f*GScale);
	}
	setWater("snow");
}

void CWaterTask::update()
{
}

void CWaterTask::render()
{
}

void CWaterTask::release()
{
	CSceneUser *su = dynamic_cast<CSceneUser *>(&C3DTask::instance().scene());
	CScene		&scene = su->getScene ();

	if(!WaterMesh.empty())
	{
		C3DTask::instance().scene().deleteInstance(WaterMesh);
	}

	if(WaterInstance)
	{
		scene.deleteInstance(WaterInstance);
		WaterInstance = 0;
	}
}
