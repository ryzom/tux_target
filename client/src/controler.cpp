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

#include "stdpch.h"

#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/misc/bitmap.h>

#include "task_manager.h"
#include "3d_task.h"
#include "editor_task.h"
#include "time_task.h"
#include "controler.h"
#include "mtp_target.h"
#include "network_task.h"
#include "level_manager.h"
#include "entity_manager.h"

using namespace std;
using namespace NLMISC;
using namespace NL3D;

#define PIQUE_SPEED (0.05f)
#define ROT_SPEED_OPEN (0.05f)
#define ROT_SPEED_CLOSE (0.5f)

static bool			 CaptureState = false;

CQuat ControlerFreeLookRot(0,0,0,0);
CVector ControlerFreeLookPos(0,0,0);
CMatrix ControlerCamMatrix;


CControler::CControler()
{
	reset();
}

void CControler::reset()
{
	EId = 255;

	FirstFrame = true;
	Accel = CVector(0,0,0);
	VirtualFrameTime = 0.0f;
	ViewedEId = 0;

	MissedTime = 0.0f;
	RotZ = 0.0f;
	Pique = 0.0f;
	Camera.reset();
	Camera.setFollowedEntity(255);
}

CControler::~CControler()
{
}

void CControler::setControledEntity(uint8 eid)
{
	EId = eid;
	//camera.setFollowedEntity(entity);
}

uint8 CControler::getControledEntity() const
{
	return EId;
}

void CControler::swapOpenClose()
{
	CVector dir = CEntityManager::getInstance()[EId].interpolator().currentDirection();
	dir.z = 0;
	dir.normalize();
	
	RotZ = (float)acos (dir.y);
	if(dir.x>0.0f)
		RotZ = -RotZ;
	RotZ += (float)NLMISC::Pi;
	
	//RotZ  = 0;
	Pique = 1.0f;
	Accel = CVector::Null;
	MissedTime = 0;
	VirtualFrameTime = 0.0f;
}

void CControler::update()
{
	if(EId==255)
		return;

	if(FirstFrame)
	{
		FirstFrame = false;
		Accel = CVector::Null;
		VirtualFrameTime = 0.0f;
	}

	CVector	deltaAccel(CVector::Null);
	CVector	deltaDirection(CVector::Null);
	float	deltaPique = 0.0f;
	float	deltaRot = 0.0f;
	float	speed = CEntityManager::getInstance()[EId].interpolator().currentSpeed().norm();
	float	speedRatio = 2.0f;
	{
		CMatrix *mat = Camera.getMatrixNoZoom();
		CVector lookAt;
		CVector right;
		lookAt = mat->getJ();
		right  = mat->getI();
		/*
		if(lookAt.norm()>1.0f)
			nlinfo(">>look = %f",lookAt.norm());
		if(right.norm()>1.0f)
			nlinfo(">>right = %f",right.norm());
		*/
		if (!FollowEntity && !CEditorTask::getInstance().enable())
		{			
			CVector dv(0,0,0);
			if (C3DTask::getInstance().kbDown(KeyUP))
			{
				dv.y += 1.0f;
			}
			if (C3DTask::getInstance().kbDown(KeyDOWN))
			{
				dv.y -= 1.0f;
			}
			if (C3DTask::getInstance().kbDown(KeyLEFT))
			{
				dv.x -= 1.0f;
			}
			if (C3DTask::getInstance().kbDown(KeyRIGHT))
			{
				dv.x += 1.0f;
			}
			if (C3DTask::getInstance().kbDown(KeyPRIOR))
			{
				dv.z += 1.0f;
			}
			if (C3DTask::getInstance().kbDown(KeyNEXT))
			{
				dv.z -= 1.0f;
			}

			dv /= 4.0f;

			if (C3DTask::getInstance().kbDown(KeySHIFT))
				dv *= 13.0f;

			if (C3DTask::getInstance().kbDown(KeyCONTROL))
				dv *= 40.0f;
			
			dv *= (float)CTimeTask::getInstance().deltaTime();

			float mouseX = C3DTask::getInstance().mouseListener().MouseX;
			float mouseY = -C3DTask::getInstance().mouseListener().MouseY;
			
			CMatrix m2;
			m2.identity();
			m2.rotate(ControlerFreeLookRot);
			m2.rotateZ(mouseX);
			m2.rotateX(mouseY);
			dv = m2 * dv;
			ControlerFreeLookPos += dv;

			ControlerCamMatrix.identity();
			ControlerCamMatrix.translate(ControlerFreeLookPos);
			ControlerCamMatrix.rotate(ControlerFreeLookRot);
			ControlerCamMatrix.rotateZ(mouseX);
			ControlerCamMatrix.rotateX(mouseY);
			//nlinfo("set camera matrix q = %f %f %f : %f",ControlerFreeLookRot.getAxis().x,ControlerFreeLookRot.getAxis().y,ControlerFreeLookRot.getAxis().z,ControlerFreeLookRot.getAngle());
			C3DTask::getInstance().scene().getCam().setMatrix(ControlerCamMatrix);
		}
		else
		{
			if(!CEntityManager::getInstance()[EId].openClose())
			{
				/*
				if (C3DTask::getInstance().kbDown(mtLEFT))
					deltaRot += ROT_SPEED_CLOSE;
				if (C3DTask::getInstance().kbDown(mtRIGHT))
					deltaRot -= ROT_SPEED_CLOSE;
				*/
				if(speed>1.0f)
				{
					if (C3DTask::getInstance().kbDown(KeyLEFT))
						deltaAccel -= right * speed * speedRatio;
					if (C3DTask::getInstance().kbDown(KeyRIGHT))
						deltaAccel += right * speed * speedRatio;	
				}
				else
				{
					if (C3DTask::getInstance().kbDown(KeyLEFT))
						deltaAccel -= right ;
					if (C3DTask::getInstance().kbDown(KeyRIGHT))
						deltaAccel += right ;					
				}

				if (C3DTask::getInstance().kbDown(KeyUP))
					deltaAccel += lookAt;
				if (C3DTask::getInstance().kbDown(KeyDOWN))
					deltaAccel -= lookAt;
			}
			else
			{
				if (C3DTask::getInstance().kbDown(KeyUP))
					deltaPique -= PIQUE_SPEED;
				if (C3DTask::getInstance().kbDown(KeyDOWN))
					deltaPique += PIQUE_SPEED;

				if (C3DTask::getInstance().kbDown(KeyLEFT))
					deltaRot += ROT_SPEED_OPEN;
				if (C3DTask::getInstance().kbDown(KeyRIGHT))
					deltaRot -= ROT_SPEED_OPEN;
			}
		}
		
		if (C3DTask::getInstance().kbPressed(KeyESCAPE))
			CTaskManager::getInstance().exit();
			//exit(0);

		if (C3DTask::getInstance().kbPressed(KeyF1))
		{
			if (C3DTask::getInstance().kbDown(KeySHIFT))
			{
//				mtpTarget::getInstance().World.switchPhysic();
				if(CLevelManager::getInstance().levelPresent())
					CLevelManager::getInstance().currentLevel().switchStartPositions();
			}
			else
			{
//				mtpTarget::getInstance().World.switchLevel();
				if(CLevelManager::getInstance().levelPresent())
					CLevelManager::getInstance().currentLevel().switchLevel();
			}
		}

		if (C3DTask::getInstance().kbPressed(KeyF3))
		{
			UDriver::TPolygonMode p = C3DTask::getInstance().driver().getPolygonMode ();
			p = UDriver::TPolygonMode(((int)p+1)%3);
			C3DTask::getInstance().driver().setPolygonMode (p);
		}

		if (C3DTask::getInstance().kbDown(KeyCONTROL) && C3DTask::getInstance().kbPressed(KeyF4))
		{
			if (C3DTask::getInstance().kbDown(KeySHIFT))
			{
//				mtpTarget::getInstance().World.switchStartPosition();
			}
			else
			{
				DisplayDebug = !DisplayDebug;
			}
		}

		if (C3DTask::getInstance().kbDown(KeyCONTROL))
		{
			if (C3DTask::getInstance().kbPressed(KeyF5))
				CNetworkTask::getInstance().command("forceEnd");
			//mtNetSendCommand("forceEnd");
			
			if (C3DTask::getInstance().kbPressed(KeyF6))
				CNetworkTask::getInstance().command("reset");
			//mtNetSendCommand("reset");
		}
		else
		{
			if (C3DTask::getInstance().kbPressed(KeyF5))
				CNetworkTask::getInstance().command("addBot");
			//mtNetSendCommand("forceEnd");
			
			if (C3DTask::getInstance().kbPressed(KeyF6))
				CNetworkTask::getInstance().command("kick bot");
			//mtNetSendCommand("reset");
		}
		
		if (C3DTask::getInstance().kbPressed(KeyPAUSE) && !ReplayFile.empty())
		{
			CTimeTask::getInstance().reset();
		}
		if (C3DTask::getInstance().kbPressed(KeyF7))
		{
			FollowEntity = !FollowEntity;
			CMtpTarget::getInstance().controler().Camera.getMatrix()->getPos(ControlerFreeLookPos);
		}

		if (C3DTask::getInstance().kbPressed(KeyF8))
		{
			// F8 -> release/capture the mouse cursor
			if (!CaptureState)
			{
				C3DTask::getInstance().driver().setCapture(false);
				C3DTask::getInstance().driver().showCursor(true);
				C3DTask::getInstance().mouseListener().removeFromServer(C3DTask::getInstance().driver().EventServer);
			}
			else
			{
				C3DTask::getInstance().driver().setCapture(true);
				C3DTask::getInstance().driver().showCursor(false);
				C3DTask::getInstance().mouseListener().addToServer(C3DTask::getInstance().driver().EventServer);
			}
			CaptureState = !CaptureState;
		}

		if (C3DTask::getInstance().kbPressed(KeyF9))
		{
/*			ViewedEId++;
			if(ViewedEId==mtpTarget::getInstance().World.count())
				ViewedEId=0;
			CEntity *entity= mtpTarget::getInstance().World.get(ViewedEId);
			camera.setFollowedEntity(entity);
*/
			ViewedEId = CEntityManager::getInstance().findPreviousEId(ViewedEId);
			Camera.setFollowedEntity(ViewedEId);
		}

		if (C3DTask::getInstance().kbPressed(KeyF10))
		{
/*			if(ViewedEId==0)
				ViewedEId=mtpTarget::getInstance().World.count()-1;
			else
				ViewedEId--;

			CEntity *entity= mtpTarget::getInstance().World.get(ViewedEId);
			camera.setFollowedEntity(entity);
*/
			ViewedEId = CEntityManager::getInstance().findNextEId(ViewedEId);
			Camera.setFollowedEntity(ViewedEId);
		}

		if (C3DTask::getInstance().kbPressed(KeyF11))
		{
			CMtpTarget::getInstance().resetFollowedEntity();
		}

		if (C3DTask::getInstance().kbPressed(KeyF12))
		{
			UCamera c = C3DTask::getInstance().scene().getCam();
			CVector v = c.getMatrix().getPos();
			CQuat q;
			c.getMatrix().getRot(q);
			
			if(CLevelManager::getInstance().levelPresent())
				nlinfo("Add this in the '%s', in the ExternalCamera section:", CLevelManager::getInstance().currentLevel().filename().c_str());
			else
				nlinfo("no level to get ExternalCamera ");
			nlinfo("\t{ Position = CVector(%f, %f, %f), Rotation = CAngleAxis(%f, %f, %f, %f) },",
				v.x, v.y, v.z, q.x, q.y, q.z, q.w);
		}
		
		/*
		if(C3DTask::getInstance().kbDown(KeyMENU))
			nlinfo(">>KeyMENU");
		if(C3DTask::getInstance().kbDown(KeyLMENU))
			nlinfo(">>KeyLMENU");
		if(C3DTask::getInstance().kbDown(KeyRMENU))
			nlinfo(">>KeyRMENU");

		if(C3DTask::getInstance().kbPressed(KeyCONTROL))
			nlinfo(">>KeyCONTROL");
		if(C3DTask::getInstance().kbPressed(KeyLCONTROL))
			nlinfo(">>KeyLCONTROL");
		if(C3DTask::getInstance().kbPressed(KeyRCONTROL))
			nlinfo(">>KeyRCONTROL");
			*/
		

		if (C3DTask::getInstance().kbPressed(KeyLCONTROL) || C3DTask::getInstance().kbPressed(KeyRCONTROL) || C3DTask::getInstance().kbPressed(KeyCONTROL) && !C3DTask::getInstance().kbDown(KeyMENU))
		{
			//trick to remove all key control pressed event
			C3DTask::getInstance().kbPressed(KeyLCONTROL);
			C3DTask::getInstance().kbPressed(KeyRCONTROL);
			C3DTask::getInstance().kbPressed(KeyCONTROL);

			CNetworkTask::getInstance().openClose();
			//if(isLocal())
			swapOpenClose();
		}
		

		if (C3DTask::getInstance().kbDown(KeyMENU) && C3DTask::getInstance().kbPressed(KeyA))
		{
			C3DTask::getInstance().EnableExternalCamera = !C3DTask::getInstance().EnableExternalCamera;
		}
	}

	float dt = (float)CTimeTask::getInstance().deltaTime();

	VirtualFrameTime += (float)CTimeTask::getInstance().deltaTime();
	Accel     += 10.0f * deltaAccel * ( (dt + MissedTime) );
	Pique     += 10.0f * deltaPique * ( (dt + MissedTime) ); 
	RotZ      += 10.0f * deltaRot   * ( (dt + MissedTime) );
	MissedTime = 0.0f;

//	nlinfo ("pik %f", Pique);

/*	if(Pique<0.0f)
		Pique=0.0f;
	if(Pique>1.0f)
		Pique=1.0f;*/
	
	Pique = max(Pique, 0.0f);
	Pique = min(Pique, 1.0f);

	CVector rotation = CEntityManager::getInstance()[EId].interpolator().rotation();
	rotation.x  = Pique;
	//rotation.z  = RotZ;
	rotation.y  = deltaRot;
	CEntityManager::getInstance()[EId].interpolator().rotation(rotation);

	if(VirtualFrameTime >= NetVirtualFrameDuration)
	{
		//TODO gerer la diff de temps entre _virtualFrameTime et NetVirtualFrameDuration
		if(!CEntityManager::getInstance()[EId].openClose())
		{
			CVector newForce(Accel.x, Accel.y, 0.0f);
			CNetworkTask::getInstance().force(newForce);
			//nlinfo(">> force=%f",newForce.norm());
		}
		else
		{
			float realRotz = RotZ - (float)NLMISC::Pi / 2.0f;
			CVector newForce(realRotz, 0.0f, Pique);
			CNetworkTask::getInstance().force(newForce);
		}
	
		Accel = CVector::Null;
		MissedTime = VirtualFrameTime - NetVirtualFrameDuration;
		VirtualFrameTime = 0.0f;
	}

	Camera.update();
}
