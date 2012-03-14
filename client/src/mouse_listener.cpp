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

#include "global.h"

#include <nel/misc/event_server.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_visual_collision_entity.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>

#include <nel/misc/time_nl.h>
#include <nel/misc/quat.h>
#include <nel/misc/plane.h>

#include "3d_task.h"
#include "config_file_task.h"
#include "mtp_target.h"
#include "mouse_listener.h"
#include "gui.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;

//
// Variables
//

//
// Functions
//

C3dMouseListener::C3dMouseListener()
{
	MouseX = 0.0f;
	MouseY = 0.0f;
	MouseWheel = 0;
	_allowMouse = 0;
	_inverseMouse = false;
}

C3dMouseListener::~C3dMouseListener()
{
}

void C3dMouseListener::init()
{
	_allowMouse = CConfigFileTask::getInstance().configFile().getVar("AllowMouse").asInt();
	_inverseMouse = CConfigFileTask::getInstance().configFile().getVar("InverseMouse").asInt() == 1;
}

void C3dMouseListener::reset()
{
	MouseX = 0.0f;
	MouseY = 0.0f;
	MouseWheel = 0;
}


void C3dMouseListener::operator ()(const CEvent& event)
{
	if(CGuiObjectManager::getInstance().objects.size()!=0)
		return;
	
	CEventMouse* mouseEvent = (CEventMouse*)&event;

	switch (_allowMouse)
	{
	case 1:
		if (event==EventMouseMoveId && ((mouseEvent->Button&leftButton)!=0  || !FollowEntity))
		{
			MouseX += 0.5f - mouseEvent->X;
			if (_inverseMouse)
				MouseY -= 0.5f - mouseEvent->Y;
			else
				MouseY += 0.5f - mouseEvent->Y;
		}
		else if (event==EventMouseWheelId)
		{
			CEventMouseWheel* mouseEvent=(CEventMouseWheel*)&event;
			MouseWheel += (mouseEvent->Direction? -1 : +1);
		}
		// Update mouse position
		C3DTask::getInstance().driver().setMousePos(0.5f, 0.5f);
		break;
	case 2:
		if (event==EventMouseMoveId && ((mouseEvent->Button&leftButton)!=0 || !FollowEntity) )
		{
			MouseX += 0.5f - mouseEvent->X;
			if (_inverseMouse)
				MouseY -= 0.5f - mouseEvent->Y;
			else
				MouseY += 0.5f - mouseEvent->Y;
		}
		else if (event == EventMouseUpId)
		{
			MouseX = 0.0f;
			MouseY = 0.0f;
			MouseWheel = 0;
		}
		else if (event == EventMouseWheelId)
		{
			CEventMouseWheel* mouseEvent=(CEventMouseWheel*)&event;
			MouseWheel += (mouseEvent->Direction? -1 : +1);
		}
		// Update mouse position
		C3DTask::getInstance().driver().setMousePos(0.5f, 0.5f);
		break;
	case 3:
		if (event==EventMouseMoveId)
		{
			MouseX += 0.5f - mouseEvent->X;
			if (_inverseMouse)
				MouseY -= 0.5f - mouseEvent->Y;
			else
				MouseY += 0.5f - mouseEvent->Y;
			
			// Update mouse position
			C3DTask::getInstance().driver().setMousePos(0.5f, 0.5f);
		}
		else if (event == EventMouseUpId)
		{
			MouseX = 0.0f;
			MouseY = 0.0f;
			MouseWheel = 0;
		}
		else if (event==EventMouseWheelId)
		{
			CEventMouseWheel* mouseEvent=(CEventMouseWheel*)&event;
			MouseWheel += (mouseEvent->Direction? -1 : +1);
		}
		break;
	default:
		MouseX = 0.0f;
		MouseY = 0.0f;
		MouseWheel = 0;
		break;
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
