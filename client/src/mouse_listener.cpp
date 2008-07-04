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

#include <nel/3d/u_driver.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/event_server.h>
#include <nel/3d/u_visual_collision_entity.h>

#include <nel/misc/quat.h>
#include <nel/misc/plane.h>
#include <nel/misc/time_nl.h>

#include "gui.h"
#include "3d_task.h"
#include "mtp_target.h"
#include "mouse_listener.h"
#include "config_file_task.h"
#include "../../common/constant.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

static CVariable<float> MouseXAccel("mtp", "MouseXAccel", "", 1.0f, 0, true);
static CVariable<float> MouseYAccel("mtp", "MouseYAccel", "", 1.0f, 0, true);
static CVariable<bool>  InverseMouse("mtp", "InverseMouse", "", false, 0, true);


//
// Functions
//

C3dMouseListener::C3dMouseListener()
{
	MouseX = 0.0f;
	MouseY = 0.0f;
	MouseWheel = 0;
}

C3dMouseListener::~C3dMouseListener()
{
}

void C3dMouseListener::init()
{
	C3DTask::instance().driver().setCapture(false);
	C3DTask::instance().driver().showCursor(true);
}

float C3dMouseListener::mouseX() const { return MouseX * MouseXAccel; }
float C3dMouseListener::mouseY() const { return MouseY * MouseYAccel; }

void C3dMouseListener::reset()
{
	//if(FollowEntity)
	{
//		nlinfo("reset mouse");
		MouseX = 0.0f;
		MouseY = 0.0f;
		MouseWheel = 0;
	}
}


void C3dMouseListener::operator ()(const CEvent& event)
{
	if(!CGuiObjectManager::instance().empty())
		return;
	
	CEventMouse* mouseEvent = (CEventMouse*)&event;


	if(event == EventMouseUpId && (mouseEvent->Button&rightButton)!=0)
	{
		// right click, reset the zoom
		MouseWheel = 0;
	}

	static float pressposx = 0.0f, pressposy = 0.0f;
	static bool pressed = false;

	if (event==EventMouseMoveId && (mouseEvent->Button&leftButton)!=0 )
	{
		if(!pressed)
		{
			C3DTask::instance().driver().setCapture(true);
			C3DTask::instance().driver().showCursor(false);
			pressposx = mouseEvent->X;
			pressposy = mouseEvent->Y;
			pressed = true;
			if(ReplayFile.empty() && FollowEntity) MouseX = MouseY = 0.0f;
		}
		else if(pressposx != mouseEvent->X && pressposy != mouseEvent->Y)
		{
			MouseX += 0.5f - mouseEvent->X;
			if (InverseMouse)
				MouseY -= 0.5f - mouseEvent->Y;
			else
				MouseY += 0.5f - mouseEvent->Y;
		}
		// Update mouse position
		C3DTask::instance().driver().setMousePos(0.5f, 0.5f);
	}
	else if (event == EventMouseUpId)
	{
		if(ReplayFile.empty() && FollowEntity) MouseX = MouseY = 0.0f;

		if(pressed)
		{
			C3DTask::instance().driver().setCapture(false);
			C3DTask::instance().driver().showCursor(true);
			C3DTask::instance().driver().setMousePos(pressposx, pressposy);
			pressed = false;
		}
		//MouseWheel = 0;
	}
	else if (event == EventMouseWheelId)
	{
		CEventMouseWheel* mouseEvent2=(CEventMouseWheel*)&event;
		MouseWheel += (mouseEvent2->Direction? -1 : +1);
	}
}

void C3dMouseListener::addToServer (CEventServer& server)
{
	server.addListener (EventMouseMoveId, this);
	server.addListener (EventMouseDownId, this);
	server.addListener (EventMouseUpId, this);
	server.addListener (EventMouseWheelId, this);
	_AsyncListener.addToServer (server);
}

void C3dMouseListener::removeFromServer (CEventServer& server)
{
	server.removeListener (EventMouseMoveId, this);
	server.removeListener (EventMouseDownId, this);
	server.removeListener (EventMouseUpId, this);
	server.removeListener (EventMouseWheelId, this);
	_AsyncListener.removeFromServer (server);
}
