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

#include <nel/misc/variable.h>

#include "level.h"
#include "3d_task.h"
#include "sky_task.h"
#include "font_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "external_camera_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

static CVariable<float> DistToFollowInExternalCam("mtp", "DistToFollowInExternalCam", "", 10.0f, 0, true);
static CVariable<bool> DisplayExternalCamera("mtp", "DisplayExternalCamera", "", 1, 0, true);


//
// Functions
//

void CExternalCameraTask::init()
{
	EnableExternalCamera = false;
	FollowEntities = true;
}

static CVariable<float> a_x1("mtp", "x1", "", 590);
static CVariable<float> a_y1("mtp", "y1", "", 350);
static CVariable<float> a_w1("mtp", "w1", "", 200);
static CVariable<float> a_h1("mtp", "h1", "", 150);
static CVariable<float> a_x2("mtp", "x2", "", 587);
static CVariable<float> a_y2("mtp", "y2", "", 347);
static CVariable<float> a_w2("mtp", "w2", "", 206);
static CVariable<float> a_h2("mtp", "h2", "", 156);
static CVariable<float> _sc("mtp", "sc", "", 4.0f);

void CExternalCameraTask::render()
{
	if(!DisplayExternalCamera || !EnableExternalCamera || !CLevelManager::instance().levelPresent() || CLevelManager::instance().currentLevel().ExternalCameras.empty())
		return;

	CMatrix oldmat = C3DTask::instance().scene().getCam().getMatrix();

	float w = 800.0f;
	float h = 600.0f;

	CViewport vp;
	CScissor s;
	vp.init(a_x1/w,a_y1/h,a_w1/w,a_h1/h);
	s.init(a_x2/w,a_y2/h,a_w2/w,a_h2/h);

	// check if there s somebody near you

	CVector MyPos = CEntityManager::instance()[CMtpTarget::instance().controler().getControledEntity()].interpolator().position();

	float mindist = 99999.0f;
	CVector mindistpos;
	for(uint i = 0; i < 256; i++)
	{
		CEntity *e = CEntityManager::instance().entities()[i];
		if(e->type() != CEntity::Unknown && e->id() != CMtpTarget::instance().controler().getControledEntity() && e->ArrivalTime == 0.0f && !e->collided())
		{
			CVector tmp = MyPos - e->interpolator().position();
			if(tmp.norm() < mindist && e->interpolator().position().z+0.1f >= MyPos.z)
			{
				mindist = tmp.norm();
				mindistpos = e->interpolator().position();
			}
		}
	}

	CMatrix m;
	m.identity();

	if(FollowEntities && mindist < DistToFollowInExternalCam)
	{
		lookAt(m, MyPos+CVector(0.0f,0.0f,0.04f), mindistpos, CVector(0.0f,0.0f,1.0f));
	}
	else
	{
		m.setPos(CLevelManager::instance().currentLevel().ExternalCameras[0].first);
		m.setRot(CLevelManager::instance().currentLevel().ExternalCameras[0].second);
	}

	C3DTask::instance().scene().getCam().setMatrix(m);
	C3DTask::instance().scene().setViewport(vp);
	CSkyTask::instance().skyScene()->getCam().setMatrix(m);
	CSkyTask::instance().skyScene()->setViewport(vp);
	C3DTask::instance().driver().setViewport(vp);
	C3DTask::instance().driver().setScissor(s);
	C3DTask::instance().driver().clearBuffers();
	C3DTask::instance().driver().enableFog(false);
	CSkyTask::instance().skyScene()->render();

	C3DTask::instance().driver().enableFog(true);
	C3DTask::instance().scene().render();

	C3DTask::instance().driver().enableFog(false);
	uint32 fs = CFontManager::instance().textContext(CFontManager::TCPLayerName).getFontSize();
	CFontManager::instance().textContext(CFontManager::TCPLayerName).setFontSize(uint32(fs*_sc));
	CEntityManager::instance().renderNames(true);
	CFontManager::instance().textContext(CFontManager::TCPLayerName).setFontSize(fs);

	vp.init(0,0,1,1);
	s.init(0,0,1,1);
	C3DTask::instance().scene().setViewport(vp);
	CSkyTask::instance().skyScene()->setViewport(vp);
	C3DTask::instance().driver().setViewport(vp);
	C3DTask::instance().driver().setScissor(s);
	C3DTask::instance().scene().getCam().setMatrix(oldmat);
	CSkyTask::instance().skyScene()->getCam().setMatrix(oldmat);
}
