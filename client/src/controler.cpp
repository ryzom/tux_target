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

#include <nel/misc/types_nl.h>

#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <windows.h>
#endif

#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/misc/bitmap.h>

#include "level.h"
#include "3d_task.h"
#include "time_task.h"
#include "controler.h"
#include "chat_task.h"
#include "mtp_target.h"
#include "editor_task.h"
#include "network_task.h"
#include "task_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "config_file_task.h"
#include "external_camera_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

static const float PiqueSpeed = 0.07f;
static const float RotSpeedOpen = 0.07f;
static const float SpeedRatio = 3.0f;

// Original values
// #define PIQUE_SPEED (0.05f)
// #define ROT_SPEED_OPEN (0.05f)
//static const float SpeedRatio = 2.0f;

static bool CaptureState = false;

CQuat ControlerFreeLookRot(0,0,0,0);
CVector ControlerFreeLookPos(CVector::Null);
CMatrix ControlerCamMatrix;


//
// Functions
//

CControler::CControler()
{
	reset();
}

void CControler::reset()
{
	EId = 255;

	FirstFrame = true;
//	Accel = CVector::Null;
	VirtualFrameTime = 0.0f;
	ViewedEId = 255;

//	MissedTime = 0.0f;
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
	CVector dir = CEntityManager::instance()[EId].interpolator().direction();
	dir.z = 0;
	dir.normalize();
	
	RotZ = (float)acos (dir.y);
	if(dir.x>0.0f)
		RotZ = -RotZ;
	RotZ += (float)NLMISC::Pi;
	
	//RotZ  = 0;
	Pique = 1.0f;
//	Accel = CVector::Null;
//	MissedTime = 0;
	VirtualFrameTime = 0.0f;
}

void CControler::updateDebugKeys()
{
	if (!C3DTask::instance().kbDown(KeySHIFT))
		return;

	if (C3DTask::instance().kbPressed(KeyF2))
	{
		UDriver::TPolygonMode p = C3DTask::instance().driver().getPolygonMode ();
		p = UDriver::TPolygonMode(((int)p+1)%3);
		C3DTask::instance().driver().setPolygonMode (p);
	}

	if (C3DTask::instance().kbPressed(KeyF3))
	{
		if(CLevelManager::instance().levelPresent())
			CLevelManager::instance().currentLevel().switchStartPositions();
	}

	if (C3DTask::instance().kbPressed(KeyF4))
		DisplayDebug = (DisplayDebug+1)%4;

	if (C3DTask::instance().kbPressed(KeyF5))
		CNetworkTask::instance().command(ucstring("addBot"));

	if (C3DTask::instance().kbPressed(KeyF6))
		CNetworkTask::instance().command(ucstring("kick bot"));

	if (C3DTask::instance().kbPressed(KeyF7))
		CNetworkTask::instance().command(ucstring("forceEnd"));

	if (C3DTask::instance().kbPressed(KeyF8))
		CNetworkTask::instance().command(ucstring("reset"));

	if (C3DTask::instance().kbPressed(KeyF9))
	{
		FollowEntity = !FollowEntity;
		CMtpTarget::instance().controler().Camera.getMatrix()->getPos(ControlerFreeLookPos);
		ControlerFreeLookRot.identity();
	}

	if (C3DTask::instance().kbPressed(KeyF12))
	{
		UCamera c = C3DTask::instance().scene().getCam();
		CVector v = c.getMatrix().getPos();
		CQuat q;
		c.getMatrix().getRot(q);

		if(CLevelManager::instance().levelPresent())
			nlinfo("Add this in the '%s', in the CLevel:init() function:", CLevelManager::instance().currentLevel().filename().c_str());
		else
			nlinfo("no level to get ExternalCamera");
		nlinfo("self:addExternalCamera(CVector(%f, %f, %f), CAngleAxis(%f, %f, %f, %f))", v.x, v.y, v.z, q.x, q.y, q.z, q.w);
	}
}

void replaySetSpeed(TKey key, double value)
{
	static bool firstDown = true;
	static TKey currentKey = KeyCount;
	static double oldSpeed = 0.0;
	if (C3DTask::instance().kbDown(key) && currentKey == KeyCount)
	{
		if(firstDown)
		{
			oldSpeed = CTimeTask::instance().getSpeedTime();
			CTimeTask::instance().speedTime(value);
			firstDown = false;
			currentKey = key;
		}
	}
	else if(!C3DTask::instance().kbDown(key) && currentKey == key)
	{
		if(!firstDown)
		{
			CTimeTask::instance().speedTime(oldSpeed);
			firstDown = true;
			currentKey = KeyCount;
		}
	}
}

void CControler::updateUserKeys()
{
	if (C3DTask::instance().kbDown(KeySHIFT))
	{
#ifdef NL_OS_WINDOWS
		if (C3DTask::instance().kbPressed(KeyINSERT))
		{
			ucstring toAppend;
			if (OpenClipboard(NULL))
			{
				HANDLE hObj=GetClipboardData(CF_UNICODETEXT);
				if (hObj)
				{
					wchar_t *sString=(wchar_t*)GlobalLock(hObj);
					if(sString != NULL)
					{
						for(unsigned int i = 0; i < wcslen(sString); i++)
						{
							ucchar ch = ucchar(sString[i]);
							if(ch == '\t') ch = ' ';
							else if(ch == '\r') continue;
							else if(ch == '\n') break;
							toAppend += ch;
						}
						GlobalUnlock (hObj);
					}
				}
				CloseClipboard ();
			}
			nlinfo ("Chat input was pasted from the clipboard: '%s'", toAppend.c_str());
			CChatTask::instance().addToInput(toAppend);
		}
#endif // NL_OS_WINDOWS

	}
	else
	{
		if(C3DTask::instance().kbPressed(KeyF1))
			CGuiObjectManager::instance().displayFrame("help1");

		// Take a screen shot
 		if(C3DTask::instance().kbPressed(KeyF2))
 			C3DTask::instance().takeScreenShot();

		if (C3DTask::instance().kbPressed(KeyF4))
			CNetworkTask::instance().command(ucstring("v"));

		// Music management keys

		if(C3DTask::instance().kbPressed(KeyF5))
			CSoundManager::instance().switchPauseMusic();

		if(C3DTask::instance().kbPressed(KeyF6))
			CSoundManager::instance().playPreviousMusic();

		if(C3DTask::instance().kbPressed(KeyF7))
			CSoundManager::instance().playNextMusic();

		// Entities following keys

		//if(FollowEntity)
		{
			if (C3DTask::instance().kbPressed(KeyF9))
			{
				FollowEntity = true;
				ViewedEId = CEntityManager::instance().findPreviousEId(ViewedEId);
				Camera.setFollowedEntity(ViewedEId);
			}

			if (C3DTask::instance().kbPressed(KeyF10))
			{
				FollowEntity = true;
				ViewedEId = CEntityManager::instance().findNextEId(ViewedEId);
				Camera.setFollowedEntity(ViewedEId);
			}

			if (C3DTask::instance().kbPressed(KeyF11))
			{
				FollowEntity = true;
				CMtpTarget::instance().resetFollowedEntity();
			}

			if(ViewedEId == CMtpTarget::instance().controler().getControledEntity() && CEntityManager::instance()[ViewedEId].spectator())
			{
				FollowEntity = false;
				//CMtpTarget::instance().controler().Camera.getMatrix()->getPos(ControlerFreeLookPos);
				//ControlerFreeLookRot.identity();
			}
		}

		if (C3DTask::instance().kbPressed(KeyF12))
			CExternalCameraTask::instance().switchExternalCamera();

		if(!ReplayFile.empty())
		{
			if (C3DTask::instance().kbPressed(KeyHOME))
			{
				CTimeTask::instance().reset();
				CMtpTarget::instance().loadReplayFile();
			}

			if (C3DTask::instance().kbPressed(KeyZ))
			{
				CTimeTask::instance().speedTime((CTimeTask::instance().getSpeedTime() != 0.0)?0.0:1.0);
			}

			if (C3DTask::instance().kbDown(KeyS))
			{
				if(CTimeTask::instance().getSpeedTime() > 0.0)
					CTimeTask::instance().speedTime(CTimeTask::instance().getSpeedTime()-0.05);
				else
					CTimeTask::instance().speedTime(0.0);
			}

			if (C3DTask::instance().kbDown(KeyX))
			{
				if(CTimeTask::instance().getSpeedTime() < 1.0)
					CTimeTask::instance().speedTime(CTimeTask::instance().getSpeedTime()+0.05);
				else
					CTimeTask::instance().speedTime(1.0);
			}

			replaySetSpeed(KeyA, -0.1);
			replaySetSpeed(KeyE, +0.1);

			replaySetSpeed(KeyQ, -3.0);
			replaySetSpeed(KeyD, +3.0);

			replaySetSpeed(KeyW, -6.0);
			replaySetSpeed(KeyC, +6.0);

/*			if (C3DTask::instance().kbPressed(KeyEND))
				CTimeTask::instance().stopStart();

			if (C3DTask::instance().kbDown(KeyCONTROL) && C3DTask::instance().kbPressed(KeyINSERT))
				CTimeTask::instance().stepBack(-1.0);

			if (C3DTask::instance().kbDown(KeyCONTROL) && C3DTask::instance().kbPressed(KeyDELETE))
				CTimeTask::instance().stepBack(+1.0);

			if (C3DTask::instance().kbPressed(KeyINSERT))
				CTimeTask::instance().incSpeedTime(+0.1);

			if (C3DTask::instance().kbPressed(KeyDELETE))
				CTimeTask::instance().incSpeedTime(-0.1);
*/
		}

		if (C3DTask::instance().kbPressed(KeyLCONTROL) || C3DTask::instance().kbPressed(KeyRCONTROL) || C3DTask::instance().kbPressed(KeyCONTROL) && !C3DTask::instance().kbDown(KeyMENU))
		{
			//trick to remove all key control pressed event
			C3DTask::instance().kbPressed(KeyLCONTROL);
			C3DTask::instance().kbPressed(KeyRCONTROL);
			C3DTask::instance().kbPressed(KeyCONTROL);

			CNetworkTask::instance().openClose();
			swapOpenClose();
		}

		/*
		if (C3DTask::instance().kbPressed(KeyFXX))
		{
		// F8 -> release/capture the mouse cursor
		if (!CaptureState)
		{
		C3DTask::instance().driver().setCapture(false);
		C3DTask::instance().driver().showCursor(true);
		C3DTask::instance().mouseListener().removeFromServer(C3DTask::instance().driver().EventServer);
		}
		else
		{
		C3DTask::instance().driver().setCapture(true);
		C3DTask::instance().driver().showCursor(false);
		C3DTask::instance().mouseListener().addToServer(C3DTask::instance().driver().EventServer);
		}
		CaptureState = !CaptureState;
		}
		*/

	}
}

void CControler::update()
{
	if(EId==255)
		return;

	if(FirstFrame)
	{
		FirstFrame = false;
//		Accel = CVector::Null;
		VirtualFrameTime = 0.0f;
	}

	CVector	deltaAccel(CVector::Null);
	CVector	deltaDirection(CVector::Null);
	float	deltaPique = 0.0f;
	float	deltaRot = 0.0f;
	float	speed = CEntityManager::instance()[EId].interpolator().speed().norm();

	if (!FollowEntity && !CEditorTask::instance().enable())
	{			
		CVector dv(0,0,0);
		if (C3DTask::instance().kbDown(KeyUP))
		{
			dv.y += 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyDOWN))
		{
			dv.y -= 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyLEFT))
		{
			dv.x -= 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyRIGHT))
		{
			dv.x += 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyPRIOR))
		{
			dv.z += 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyNEXT))
		{
			dv.z -= 1.0f;
		}

		dv /= 3.0f;

		if (C3DTask::instance().kbDown(KeySHIFT))
			dv *= 10.0f;

		if (C3DTask::instance().kbDown(KeyCONTROL))
			dv *= 30.0f;
		
		dv *= (float)CTimeTask::instance().realDeltaTime();

		float mouseX = C3DTask::instance().mouseListener().mouseX();
		float mouseY = -C3DTask::instance().mouseListener().mouseY();

		CMatrix mm;
		mm.identity();
		mm.setRot(ControlerFreeLookRot);
		mm.invert();

		CMatrix m2;
		m2.identity();
		m2.rotateZ(mouseX);
		m2.rotateX(mouseY);
		m2.rotate(ControlerFreeLookRot);
		dv = m2 * dv;
		ControlerFreeLookPos += dv;

		ControlerCamMatrix.identity();
		ControlerCamMatrix.translate(ControlerFreeLookPos);
		ControlerCamMatrix.rotateZ(mouseX);
		ControlerCamMatrix.rotate(ControlerFreeLookRot);
		ControlerCamMatrix.rotateX(mouseY);

		//nlinfo("set camera matrix q = %f %f %f : %f",ControlerFreeLookRot.getAxis().x,ControlerFreeLookRot.getAxis().y,ControlerFreeLookRot.getAxis().z,ControlerFreeLookRot.getAngle());
		C3DTask::instance().scene().getCam().setMatrix(ControlerCamMatrix);
	}
	else
	{
		if(!CEntityManager::instance()[EId].openClose())
		{
			CMatrix *mat = Camera.getMatrixNoZoom();
			CVector lookAt = mat->getJ().normed();
			CVector right = mat->getI().normed();

			if(speed>1.0f)
			{
				if (C3DTask::instance().kbDown(KeyLEFT))
					deltaAccel -= right * speed * SpeedRatio;
				if (C3DTask::instance().kbDown(KeyRIGHT))
					deltaAccel += right * speed * SpeedRatio;	
			}
			else
			{
				if (C3DTask::instance().kbDown(KeyLEFT))
					deltaAccel -= right;
				if (C3DTask::instance().kbDown(KeyRIGHT))
					deltaAccel += right;					
			}

			if (C3DTask::instance().kbDown(KeyUP))
				deltaAccel += lookAt;
			if (C3DTask::instance().kbDown(KeyDOWN))
				deltaAccel -= lookAt;
		}
		else
		{
			if (C3DTask::instance().kbDown(KeyUP))
				deltaPique -= PiqueSpeed;
			if (C3DTask::instance().kbDown(KeyDOWN))
				deltaPique += PiqueSpeed;

			if (C3DTask::instance().kbDown(KeyLEFT))
				deltaRot += RotSpeedOpen;
			if (C3DTask::instance().kbDown(KeyRIGHT))
				deltaRot -= RotSpeedOpen;
		}
	}

	float dt = float(CTimeTask::instance().deltaTime());

	VirtualFrameTime += float(CTimeTask::instance().deltaTime());
//	Accel     += 10.0f * deltaAccel * dt;//* ( (dt + MissedTime) );
 	Pique     += 10.0f * deltaPique * dt;//* ( (dt + MissedTime) ); 
 	RotZ      += 10.0f * deltaRot   * dt;//* ( (dt + MissedTime) );
//	MissedTime = 0.0f;

 	Pique = max(Pique, 0.0f);
 	Pique = min(Pique, 1.0f);

 	CVector rotation = CEntityManager::instance()[EId].interpolator().rotation();
 	rotation.x  = Pique;
 	CEntityManager::instance()[EId].interpolator().setRotation(rotation);

	//if(VirtualFrameTime >= NetworkUpdatePeriod)
	{
		if(!CEntityManager::instance()[EId].openClose())
		{
			//CVector newForce(Accel.x, Accel.y, 0.0f);
			CVector newForce(deltaAccel.x, deltaAccel.y, deltaAccel.z);
			newForce /= 4.0f;
			CNetworkTask::instance().setForce(newForce);
		}
		else
		{
			float realRotz = RotZ - float(NLMISC::Pi) / 2.0f;
			// we set Y with a dumb value to be able to detect on the server that the force is a good open force
			CVector newForce(realRotz, 111.0f, Pique);
			CNetworkTask::instance().setForce(newForce);
		}

//		Accel = CVector::Null;
//		MissedTime = VirtualFrameTime - NetworkUpdatePeriod;
		VirtualFrameTime -= NetworkUpdatePeriod;
	}

	updateDebugKeys();
	updateUserKeys();

	Camera.update();
}
