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

#include <nel/3d/u_scene.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_cloud_scape.h>
#include <nel/3d/u_text_context.h>

#include <nel/3d/nelu.h>
#include <nel/3d/scene.h>
#include "time_task.h"
#include <nel/3d/scene_user.h>
#include <nel/3d/water_model.h>
#include <nel/3d/water_shape.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_blend.h>
#include <nel/3d/transform_shape.h>
#include <nel/3d/water_height_map.h>
#include <nel/3d/water_pool_manager.h>

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
#include "resource_manager2.h"
#include "config_file_task.h"


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


CWaterTask::CWaterTask():ITask()
{
	envMap0Name("");
	envMap1Name("");
	heightMap0Name("");
	heightMap1Name("");

	nelWaterScene   = 0;
	WaterShape    = 0;
	WaterModel    = 0;
	WaterMesh     = 0;	
	WaterInstance = 0;	
}

void cbVar (CConfigFile::CVar &var)
{
	if (CWaterTask::getInstance().WaterShape == 0)
	{
		nlwarning ("Can't set water parameter");
		return;
	}
	
	if (var.Name == "Map1Scale")
		CWaterTask::getInstance().WaterShape->setHeightMapScale(0, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "Map2Scale")
		CWaterTask::getInstance().WaterShape->setHeightMapScale(1, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "Map1Speed")
		CWaterTask::getInstance().WaterShape->setHeightMapSpeed(0, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "Map2Speed")
		CWaterTask::getInstance().WaterShape->setHeightMapSpeed(1, NLMISC::CVector2f(var.asFloat(0), var.asFloat(1)));
	else if (var.Name == "WaveHeightFactor")
		CWaterTask::getInstance().WaterShape->setWaveHeightFactor(var.asFloat());
	else if (var.Name == "WavesParams")
	{
		CWaterHeightMap &whm = GetWaterPoolManager().getPoolByID(0);
		whm.setWaves (var.asFloat(0), var.asFloat(1), (uint)var.asFloat(2), var.asFloat(3) == 1.0f);
	}
	else if (var.Name == "WavesPropagation")
	{
		CWaterHeightMap &whm = GetWaterPoolManager().getPoolByID(0);
		whm.setPropagationTime(var.asFloat());
	}
	else
		nlstop;
}


void CWaterTask::init()
{
	nelWaterScene = C3DTask::getInstance().driver().createScene(false);

	nelWaterScene->getCam().setPerspective(degToRad(CConfigFileTask::getInstance().configFile().getVar("Fov").asFloat()), 1.33f, 1.0f*GScale, 30000.0f*GScale);
	nelWaterScene->getCam().setTransformMode (UTransformable::DirectMatrix);

////
	if(CConfigFileTask::getInstance().configFile().getVar("DisplayWater").asInt() == 2)
	{
		string res;
		static std::string shapeName("water_quad.shape");
		CSceneUser *su = dynamic_cast<CSceneUser *>(&C3DTask::getInstance().scene());
//		CSceneUser *su = dynamic_cast<CSceneUser *>(nelWaterScene);
		CScene		&scene = su->getScene ();

		// load textures
		res = CResourceManager::getInstance().get(envMap0Name());
		ITexture *envMap1   = new CTextureFile(res);
		res = CResourceManager::getInstance().get(envMap1Name());
		ITexture *envMap2   = new CTextureFile(res);
		
		res = CResourceManager::getInstance().get(heightMap0Name());
		ITexture *heightMap  = new CTextureFile(res);
		res = CResourceManager::getInstance().get(heightMap1Name());
		ITexture *heightMap2 = new CTextureFile(res);

		res = CResourceManager::getInstance().get("colormap.tga");
		ITexture *colorMap = new CTextureFile(res);
		
		WaterShape = new CWaterShape;
		WaterShape->setEnvMap(0, envMap1);
		WaterShape->setEnvMap(1, envMap2);
		
		WaterShape->setHeightMap(0, heightMap);
		WaterShape->setHeightMap(1, heightMap2);

		//WaterShape->setColorMap(colorMap);

		WaterShape->setWaterPoolID(0);

		CWaterHeightMap &whm = GetWaterPoolManager().getPoolByID(0);
//		float res = whm.getUnitSize();
//		res /= 10.0f;
//		whm.setUnitSize(res);
		whm.enableWaves ();

		float res2 = whm.getPropagationTime ();

		CConfigFileTask::getInstance().configFile().setCallback("Map1Scale", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("Map1Scale"));
		CConfigFileTask::getInstance().configFile().setCallback("Map2Scale", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("Map2Scale"));
		CConfigFileTask::getInstance().configFile().setCallback("Map1Speed", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("Map1Speed"));
		CConfigFileTask::getInstance().configFile().setCallback("Map2Speed", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("Map2Speed"));
		CConfigFileTask::getInstance().configFile().setCallback("WaveHeightFactor", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("WaveHeightFactor"));
		CConfigFileTask::getInstance().configFile().setCallback("WavesParams", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("WavesParams"));
		CConfigFileTask::getInstance().configFile().setCallback("WavesPropagation", cbVar);
		cbVar (CConfigFileTask::getInstance().configFile().getVar("WavesPropagation"));

//		WaterShape->setHeightMapScale(0, NLMISC::CVector2f(0.08f/2, 0.072f/2));
//		WaterShape->setHeightMapScale(1, NLMISC::CVector2f(0.1f/2, 0.091f/2));
//		WaterShape->setHeightMapSpeed(0, NLMISC::CVector2f(0.06f, 0.051f));
//		WaterShape->setHeightMapSpeed(1, NLMISC::CVector2f(-0.0623f, 0.085f));
//		WaterShape->setWaveHeightFactor(3);

		const float wqSize = 10000.0f;
		CVector2f corners[] =	{ CVector2f(-wqSize, wqSize), CVector2f(wqSize, wqSize),
								  CVector2f(wqSize, -wqSize), CVector2f(-wqSize, -wqSize)
								};
		CPolygon2D waterPoly;
		waterPoly.Vertices.resize(4);
		std::copy(corners, corners + 4, waterPoly.Vertices.begin());
		WaterShape->setShape(waterPoly);

		scene.getShapeBank()->add(shapeName, WaterShape);

		WaterInstance = scene.createInstance(shapeName);
		WaterModel = NLMISC::safe_cast<CWaterModel *>(WaterInstance);
		WaterModel->setPos(0.0f,0.0f,1.0f*GScale);

		//	CWaterHeightMap &whm = GetWaterPoolManager().getPoolByID(0);
		//	whm.enableWaves ();
		//	whm.setWaves (3.0f, 1.0f, 2.0f, false);
	}
	else if(CConfigFileTask::getInstance().configFile().getVar("DisplayWater").asInt() == 1)
	{
		string res;
		res = CResourceManager::getInstance().get("water_light.shape");
		WaterMesh = C3DTask::getInstance().scene().createInstance(res);
		WaterMesh.setPos(0.0f,0.0f,1.0f*GScale);
	}

}

void CWaterTask::update()
{
	/*
	CMatrix waterCameraMatrix;
	waterCameraMatrix.identity();
	waterCameraMatrix = C3DTask::getInstance().scene().getCam()->getMatrix();
	waterCameraMatrix.setPos(CVector::Null);
	*/
	
	nelWaterScene->getCam().setMatrix(C3DTask::getInstance().scene().getCam().getMatrix());
	
	nelWaterScene->animate (CTimeTask::getInstance().time());
	
}

void CWaterTask::render()
{
	C3DTask::getInstance().driver().enableFog(true);
	nelWaterScene->render ();
	C3DTask::getInstance().driver().enableFog(false);
	
	// Must clear ZBuffer For incoming rendering.
	//C3DTask::getInstance().driver().clearZBuffer();
}

void CWaterTask::release()
{
	CSceneUser *su = dynamic_cast<CSceneUser *>(&C3DTask::getInstance().scene());
//	CSceneUser *su = dynamic_cast<CSceneUser *>(nelWaterScene);
	CScene		&scene = su->getScene ();

	if(WaterInstance)
	{
		scene.deleteInstance(WaterInstance);
		WaterInstance = 0;
		//delete WaterShape;
	}

	if(nelWaterScene)
	{
		C3DTask::getInstance().driver().deleteScene(nelWaterScene);
		nelWaterScene = 0;
	}

	
}


void CWaterTask::envMap0Name(const std::string &envMapName)
{
	EnvMap0Name = "water_env.tga";
	if(envMapName.empty()) return;
	EnvMap0Name = envMapName;
}

void CWaterTask::envMap1Name(const std::string &envMapName)
{
	EnvMap1Name = "water_env.tga";
	if(envMapName.empty()) return;
	EnvMap1Name = envMapName;
}

void CWaterTask::heightMap0Name(const std::string &heightMap)
{
	HeightMap0Name = "water_disp.tga";
	if(heightMap.empty()) return;
	HeightMap0Name = heightMap;
}

void CWaterTask::heightMap1Name(const std::string &heightMap)
{
	HeightMap1Name = "water_disp2.tga";
	if(heightMap.empty()) return;
	HeightMap1Name = heightMap;
}



string CWaterTask::envMap0Name()
{
	return EnvMap0Name;
}

string CWaterTask::envMap1Name()
{
	return EnvMap1Name;
}

string CWaterTask::heightMap0Name()
{
	return HeightMap0Name;
}

string CWaterTask::heightMap1Name()
{
	return HeightMap1Name;
}

