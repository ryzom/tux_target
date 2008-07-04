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

#include <float.h>

#include <nel/misc/debug.h>
#include <nel/misc/common.h>

#include "level.h"
#include "camera.h"
#include "3d_task.h"
#include "hud_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "config_file_task.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Variables
//

static CVariable<float> OpenHeight("mtp", "OpenHeight", "", 1.0f, 0, true);
static CVariable<float> OpenBackDist("mtp", "OpenBackDist", "", 1.0f, 0, true);
static CVariable<float> OpenTargetHeight("mtp", "OpenTargetHeight", "", 1.0f, 0, true);
static CVariable<float> OpenTargetBackDist("mtp", "OpenTargetBackDist", "", 1.0f, 0, true);
static CVariable<float> CloseHeight("mtp", "CloseHeight", "", 1.0f, 0, true);
static CVariable<float> CloseBackDist("mtp", "CloseBackDist", "", 1.0f, 0, true);
static CVariable<float> CloseTargetHeight("mtp", "CloseTargetHeight", "", 1.0f, 0, true);
static CVariable<float> CloseTargetBackDist("mtp", "CloseTargetBackDist", "", 1.0f, 0, true);
static CVariable<float> RotationSpeed("mtp", "RotationSpeed", "", 1.0f, 0, true);


//
// Functions
//

#define STAT 0

#if STAT
FILE *filestat = 0;
#endif

void lookAt(CMatrix &m, const CVector &From, const CVector &At, const CVector &Up)
{
	CVector vectorY(At - From); // y vector
	vectorY.normalize();

	// Get the dot product, and calculate the projection of the y basis vector onto the up vector. The projection is the z basis vector.
	CVector vectorZ(Up - vectorY * (Up * vectorY));

	vectorZ.normalize();

	// the x basis vector
	CVector vectorX = vectorY ^ vectorZ;

	m.identity();
	m.setRot(vectorX, vectorY, vectorZ);
	m.setPos(From);
	m.scale (1.0f);
}


CCamera::CCamera()
{
	EId = 255;
	InitialPosition = CVector(0.0f, 1000.0f*GScale, 1000.0f*GScale);

	reset();

	ForcedSpeed = 1.0f;

// 	OpenHeight			= CConfigFileTask::instance().configFile().getVar("OpenHeight").asFloat() * GScale;
// 	OpenBackDist		= CConfigFileTask::instance().configFile().getVar("OpenBackDist").asFloat() * GScale;
// 	OpenTargetHeight	= CConfigFileTask::instance().configFile().getVar("OpenTargetHeight").asFloat() * GScale;
// 	OpenTargetBackDist	= CConfigFileTask::instance().configFile().getVar("OpenTargetBackDist").asFloat() * GScale;
// 	CloseHeight			= CConfigFileTask::instance().configFile().getVar("CloseHeight").asFloat() * GScale;
// 	CloseBackDist		= CConfigFileTask::instance().configFile().getVar("CloseBackDist").asFloat() * GScale;
// 	CloseTargetHeight	= CConfigFileTask::instance().configFile().getVar("CloseTargetHeight").asFloat() * GScale;
// 	CloseTargetBackDist	= CConfigFileTask::instance().configFile().getVar("CloseTargetBackDist").asFloat() * GScale;
// 	RotationSpeed		= CConfigFileTask::instance().configFile().getVar("RotationSpeed").asFloat();

	CurrentHeight		= CloseHeight;
	CurrentBackDist		= CloseBackDist;
	CurrentTargetHeight   = CloseTargetHeight;
	CurrentTargetBackDist = CloseTargetBackDist;
	CurrentHeightSpeed  = 0;

	ActiveMatrix = &MatrixFollow;
	Facing = 0;
	MinDistFromStartPointToMove = 0.01f;
	MinDistFromStartPointToMoveVerticaly = 0.01f;

#if STAT
	if(filestat == 0)
	{
		CFile::deleteFile("campos.csv");
		filestat = fopen ("campos.csv", "wt");
	}
#endif
}

CCamera::~CCamera()
{
#if STAT
	fclose (filestat);
#endif
}

/*void CCamera::setQuakeControl(bool on)
{
	nlinfo("CAM: set quake %d", on);
	if (on)
	{
		MatrixQuake = MatrixFollow;
		ActiveMatrix = &MatrixQuake;
	}
	else
	{
		ActiveMatrix = &MatrixFollow;
	}
}*/

void CCamera::setInitialPosition(const CVector &initialPosition)
{
	nlinfo("LEVEL: set init pos %f %f %f", initialPosition.x, initialPosition.y, initialPosition.z);
	InitialPosition = initialPosition;
	//Position = InitialPosition;
}

void CCamera::reset()
{
//	nlinfo("CAM: reset camera");
	Facing = 0;
	Position = InitialPosition;
	LastEntityPosition = CVector::Null;
	CurrentLookAt   = CVector::Null;
	/*
	if(EId != 255)
		CEntityManager::instance()[EId].interpolator().reset();
	*/
	lookAt (MatrixFollow, Position, CurrentLookAt + CVector(0.0f, 0.1f, 0.0f),CVector(0.0f,0.0f,1.0f));
	CurrentLookAt = Position + CVector(0.0f, -10.0f*GScale, 0.0f);
}

void CCamera::setFollowedEntity(uint8 eid)
{
	EId = eid;
	if(EId != 255)
	{
		if(CEntityManager::instance()[EId].interpolator().available())
		{
			Position = CEntityManager::instance()[EId].interpolator().position() + CVector(0.0f, 50.0f*GScale, 100.0f*GScale);
			CurrentLookAt = CEntityManager::instance()[EId].interpolator().position();
		}
		else
		{
			Position = InitialPosition;
			CurrentLookAt = Position + CVector(0.0f, -10.0f*GScale, 0.0f);
		}
		CHudTask::instance().setDisplayViewedName(EId);
	}
}

uint8 CCamera::getFollowedEntity()
{
	return EId;
}


static float lerp(float src,float dst,float pos)
{
	return (1.0f - pos) * src + pos * dst;
}

static float rotLerp(float rsrc,float rdst,float pos)
{
	double nrsrc = fmod((double)rsrc, 2*NLMISC::Pi) + 2*NLMISC::Pi;
	nrsrc = fmod((double)nrsrc, 2*NLMISC::Pi);
	double nrdst = fmod((double)rdst, 2*NLMISC::Pi) + 2*NLMISC::Pi;
	nrdst = fmod((double)nrdst, 2*NLMISC::Pi);
	if(nrsrc<nrdst)
	{
		if(fabs(nrdst - nrsrc) > NLMISC::Pi)
			nrdst -= 2*NLMISC::Pi;
	}
	else
	{
		if(fabs(nrsrc - nrdst) > NLMISC::Pi)
			nrsrc -= 2*NLMISC::Pi;
	}
	double ipos = 1.0f - pos;

	float res = (float) (nrdst * pos + nrsrc * ipos);
	//nlinfo("%f => %f (%f) = %f",nrsrc,nrdst,pos,res);
	return res;
}

void CCamera::update()
{
	bool updated;
	if(EId == 255) return;

	float heightSpeed = CEntityManager::instance()[EId].interpolator().smoothDirection().z / 0.5f;
	if(heightSpeed > 0.0f) heightSpeed = 0.0f;

	double deltaTime = CTimeTask::instance().deltaTime();
	float lpos = 3.0f * float(deltaTime);
	if(lpos > 1.0f) lpos = 1.0f;
	CurrentHeightSpeed = lerp(CurrentHeightSpeed,heightSpeed,lpos);

	if(CEntityManager::instance()[EId].interpolator().available())
	{
		bool openClose = CEntityManager::instance()[EId].openClose();
		CVector distFromStart = CVector::Null;
		if(CEntityManager::instance()[EId].startPointId()!=255 && CLevelManager::instance().levelPresent())
			distFromStart = CEntityManager::instance()[EId].interpolator().position() - CLevelManager::instance().currentLevel().startPosition(CEntityManager::instance()[EId].startPointId());
		if(distFromStart.norm()<MinDistFromStartPointToMoveVerticaly && !openClose)
			CurrentHeightSpeed = 0.0f;

		if(openClose)
			updated = updateRampe(OpenBackDist, OpenHeight, OpenTargetBackDist, OpenTargetHeight);
		else
			updated = updateRampe(CloseBackDist, CloseHeight - CurrentHeightSpeed, CloseTargetBackDist, CloseTargetHeight);
	}
	//if(updated)
	{
/*		if(C3DTask::instance().EnableExternalCamera)
		{
			float mindist = 99999.0f;
			CVector mindistpos;
			for(uint i = 0; i < 256; i++)
			{
				CEntity *e = CEntityManager::instance().entities()[i];
				if(e->type() != CEntity::Unknown && e->id() != EId)
				{
					CVector tmp = CEntityManager::instance()[EId].interpolator().position() - e->interpolator().position();
					if(tmp.norm() < mindist)
					{
						mindist = tmp.norm();
						CurrentLookAt = mindistpos = e->interpolator().position();
					}
				}
			}
		}
*/
		CMatrix oldFollow = MatrixFollow;

		CVector up(0.0f, 0.0f, 1.0f);
		if(CurrentLookAt != up)
			lookAt(MatrixFollow, Position, CurrentLookAt, up);
		else
			lookAt(MatrixFollow, Position, CurrentLookAt + CVector(0,0.00001f, 0), up);

#if STAT
		//if(DisplayDebug==3)
		if(EId == CMtpTarget::instance().controler().Camera.EId)
		{
			fprintf(filestat, "%f;%f;%f;%f\n", CTimeTask::instance().time(), MatrixFollow.getPos().x, MatrixFollow.getPos().y, MatrixFollow.getPos().z);
		}
#endif
	}
}

bool CCamera::updateRampe(float backDist,float height,float targetBackDist,float targetHeight)
{
	double deltaTime = CTimeTask::instance().deltaTime();
	bool res = true;
	float lpos = RotationSpeed * float(deltaTime);
	if(lpos>1.0f) lpos = 1.0f;

	CurrentBackDist			= lerp(CurrentBackDist, backDist, lpos);
	float oldCurrentHeight = CurrentHeight;
	CurrentHeight			= lerp(CurrentHeight, height, lpos);
	CurrentTargetBackDist	= lerp(CurrentTargetBackDist, targetBackDist, lpos);
	CurrentTargetHeight		= lerp(CurrentTargetHeight, targetHeight, lpos);

	bool openClose = CEntityManager::instance()[EId].openClose();
	CVector distFromStart = CVector::Null;
	if(CEntityManager::instance()[EId].startPointId()!=255 && CLevelManager::instance().levelPresent())
		distFromStart = CEntityManager::instance()[EId].interpolator().position() - CLevelManager::instance().currentLevel().startPosition(CEntityManager::instance()[EId].startPointId());
	float facing;
	if(distFromStart.norm()>MinDistFromStartPointToMove || openClose)
	{
		facing = rotLerp(Facing, (float)CEntityManager::instance()[EId].interpolator().facing(), lpos);
	}
	else
	{
		CVector dir = InitialPosition;
		dir.z = 0;
		dir.normalize();
		facing = (float)acos(dir.y);
		if(dir.x>0.0f)
			facing = - facing;
		//nlinfo("dir = %f %f %f , facing = %f",dir.x,dir.y,dir.z,facing);
	}
	//nlinfo("%f => %f (%f) = %f",_facing,_entity->_facing,lpos,facing);
	Facing = facing;

	CMatrix rotMat;
	CMatrix rotMatNoZoom;
	rotMat.identity();
	rotMat.rotateZ(Facing);

	rotMat.rotateZ(C3DTask::instance().mouseListener().mouseX());

	float mouseY = C3DTask::instance().mouseListener().mouseY();

	static const float eps = 0.1f;
	if (mouseY < -(float)NLMISC::Pi/2.0f+eps)
		mouseY = -(float)NLMISC::Pi/2.0f+eps;

	if (mouseY > (float)NLMISC::Pi/2.0f-eps)
		mouseY = (float)NLMISC::Pi/2.0f-eps;

	rotMat.rotateX(mouseY);

	if (C3DTask::instance().mouseListener().MouseWheel < 0)
		C3DTask::instance().mouseListener().MouseWheel = 0;

	//CVector zoomTans = -getMatrix()->getJ()*(float)(C3DTask::instance().mouseListener().MouseWheel)/2.0f;
	//zoomTans.x = 0;
	rotMatNoZoom = rotMat;
	CVector zoomTans = CVector(0,(float)(C3DTask::instance().mouseListener().MouseWheel)/2.0f,0);
	rotMat.translate(zoomTans);

	//float minMouseAngleToDisplayPart = 0.35f;
	//bool displayParticle = fabs(C3DTask::instance().mouseListener().mouseX())>minMouseAngleToDisplayPart || C3DTask::instance().mouseListener().mouseY()<-0.53f ||  C3DTask::instance().mouseListener().mouseY()>0.4f || C3DTask::instance().mouseListener().MouseWheel>4;

	Position = CEntityManager::instance()[EId].interpolator().position() + CurrentHeight * CVector(0,0,1) + CurrentBackDist * (rotMatNoZoom * CVector(0,1,0));
	CurrentLookAt = CEntityManager::instance()[EId].interpolator().position() + CurrentTargetHeight * CVector(0,0,1)  + CurrentTargetBackDist * (rotMatNoZoom * CVector(0,1,0));

	CVector up(0.0f, 0.0f, 1.0f);
	if((CurrentLookAt) !=up)
		lookAt(MatrixFollowNoZoom, Position, CurrentLookAt, up);
	else
		lookAt(MatrixFollowNoZoom, Position, CurrentLookAt + CVector(0,0.00001f, 0), up);

	Position = CEntityManager::instance()[EId].interpolator().position() + CurrentHeight * CVector(0,0,1) + CurrentBackDist * (rotMat * CVector(0,1,0));
	CurrentLookAt = CEntityManager::instance()[EId].interpolator().position() + CurrentTargetHeight * CVector(0,0,1)  + CurrentTargetBackDist * (rotMat * CVector(0,1,0));
	return res;
}

CMatrix *CCamera::getMatrix()
{
	return ActiveMatrix;
}

CMatrix *CCamera::getMatrixNoZoom()
{
	return &MatrixFollowNoZoom;
}

CMatrix *CCamera::getMatrixQuake()
{
	return &MatrixQuake;
}

CMatrix *CCamera::getMatrixFollow()
{
	return &MatrixFollow;
}

CVector CCamera::project(const CVector &point)
{
	return CVector::Null;
}


float CCamera::minDistFromStartPointToMove()
{
	return MinDistFromStartPointToMove;
}

void CCamera::minDistFromStartPointToMove(float dist)
{
	MinDistFromStartPointToMove = dist;
}

float CCamera::minDistFromStartPointToMoveVerticaly()
{
	return MinDistFromStartPointToMoveVerticaly;
}

void CCamera::minDistFromStartPointToMoveVerticaly(float dist)
{
	MinDistFromStartPointToMoveVerticaly = dist;
}
