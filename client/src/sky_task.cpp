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

#include "3d_task.h"
#include "sky_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
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
// Variables
//

static NL3D::UParticleSystemInstance ps;


//
// Functions
//


CSkyTask::CSkyTask() : ITask(), ShapeName("sky_snow.shape"), SkyScene(0), SkyMesh(0), CloudScape(0)
{
}

void CSkyTask::setSky(const string &name)
{
	if(!SkyMesh.empty())
	{
		SkyScene->deleteInstance(SkyMesh);
	}

	SkyMesh = SkyScene->createInstance(CPath::lookup("sky_"+name+".shape"));
	if (!SkyMesh.empty())
	{
		SkyMesh.setTransformMode (UTransformable::DirectMatrix);
		SkyMesh.setMatrix(CMatrix::Identity);
	}
}

void CSkyTask::init()
{
	SkyScene = C3DTask::instance().driver().createScene(false);
	nlassert(SkyScene);
	SkyScene->getCam().setPerspective(degToRad(CConfigFileTask::instance().configFile().getVar("Fov").asFloat()), 1.33f, 0.15f, 3000.0f);
	SkyScene->getCam().setTransformMode (UTransformable::DirectMatrix);

	setSky("snow");

	if(CConfigFileTask::instance().configFile().getVar("DisplayClouds").asInt() == 1)
	{
		CloudScape = SkyScene->createCloudScape();
		SCloudScapeSetup css;
		css.NbCloud = 50;
		css.CloudSpeed = 8.0f;
		css.WindSpeed = 1.5f;

		css.Ambient = NLMISC::CRGBA (64, 72, 132, 255);
		css.Diffuse = NLMISC::CRGBA (167, 177, 236, 255);

		//css.Ambient = NLMISC::CRGBA (120,150,155,255);
		//css.Diffuse = NLMISC::CRGBA (220,250,255,255);
		CloudScape->setNbCloudToUpdateIn80ms(1);
		CloudScape->setQuality (160.0);
		//CloudScape->setDebugQuad (true);
		CloudScape->init(&css);
	}

/*	res = CPath::lookup("sun.ps");
	ps.cast(C3DTask::instance().scene().createInstance(res));
	ps.setTransformMode (UTransformable::RotQuat);
	ps.setOrderingLayer(2);
	ps.setPos(-1000.0f, 0.0f, 1000.0f);
	ps.setScale(CVector(1.0f, 1.0f, 1.0f));
	ps.activateEmitters(true);
	ps.show();*/
}

void CSkyTask::update()
{
	CMatrix skyCameraMatrix;
	skyCameraMatrix.identity();
	skyCameraMatrix = C3DTask::instance().scene().getCam().getMatrix();
	skyCameraMatrix.setPos(CVector::Null);

	SkyScene->getCam().setMatrix(skyCameraMatrix);

	SkyScene->animate (CTimeTask::instance().time());

	if (CloudScape)
		CloudScape->anim (CTimeTask::instance().deltaTime()); // WARNING this function work with screen
}

void CSkyTask::render()
{
	C3DTask::instance().driver().clearZBuffer();
	SkyScene->render ();

	if (CloudScape)
		CloudScape->render ();
}

void CSkyTask::release()
{
	if(!SkyMesh.empty())
	{
		SkyScene->deleteInstance(SkyMesh);
	}

	if(CloudScape)
	{
		SkyScene->deleteCloudScape(CloudScape);
		CloudScape = 0;
	}

	if(SkyScene)
	{
		C3DTask::instance().driver().deleteScene(SkyScene);
		SkyScene = 0;
	}
}
