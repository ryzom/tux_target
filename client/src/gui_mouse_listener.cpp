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
#include "time_task.h"
#include "mtp_target.h"
#include "gui_object.h"
#include "gui_mouse_listener.h"
#include "config_file_task.h"

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

CGuiMouseListener::CGuiMouseListener()
{
	MouseX = 0.0f;
	MouseY = 0.0f;
	MouseWheel = 0;
	PressedX = 0.0f;
	PressedY = 0.0f;
	
	ButtonDown = false;
	RightButtonDown = false;
	Clicked = false;
	LastClickedTime = CTimeTask::getInstance().time();
	DoubleClicked = false;
	LastButtonDown = false;   
	_captureCursor = false;
	_cursorCaptured = _captureCursor;
}

CGuiMouseListener::~CGuiMouseListener()
{
}

void CGuiMouseListener::init()
{
}

void CGuiMouseListener::update()
{
	Pressed = ButtonDown && !LastButtonDown;
	Clicked = !ButtonDown && LastButtonDown;
	double dtime = CTimeTask::getInstance().time()-LastClickedTime ;
	DoubleClicked = Clicked && dtime<0.3f;
	if(Clicked)
		LastClickedTime = CTimeTask::getInstance().time();
	
	LastButtonDown = ButtonDown;
	
	bool shouldCaptureCursor = CGuiObjectManager::getInstance().objects.size()==0 && _captureCursor;
	if(shouldCaptureCursor != _cursorCaptured)
	{
		if(shouldCaptureCursor)
		{
			C3DTask::getInstance().driver().setCapture(true);
			C3DTask::getInstance().driver().showCursor(false);
			C3DTask::getInstance().mouseListener().addToServer(C3DTask::getInstance().driver().EventServer);
		}
		else
		{
			C3DTask::getInstance().driver().setCapture(false);
			C3DTask::getInstance().driver().showCursor(true);
			C3DTask::getInstance().mouseListener().removeFromServer(C3DTask::getInstance().driver().EventServer);
		}
		_cursorCaptured = shouldCaptureCursor;
	}
	
}


void CGuiMouseListener::operator ()(const CEvent& event)
{
	CEventMouse* mouseEvent = (CEventMouse*)&event;

	if (event==EventMouseMoveId)
	{
		MouseX = mouseEvent->X;
		MouseY = 1.0f - mouseEvent->Y;
	}
	if(event == EventMouseDownId)
	{
		if (mouseEvent->Button&leftButton)
		{
			ButtonDown = true;
			PressedX = MouseX;
			PressedY = MouseY;
		}
		else if (mouseEvent->Button&rightButton)
		{
			RightButtonDown = true;
		}
	}
	else if (event == EventMouseUpId)
	{
		if(mouseEvent->Button&leftButton)
		{
			ButtonDown = false;
		}
		else if (mouseEvent->Button&rightButton)
		{
			RightButtonDown = false;
		}
	}
	else if (event==EventMouseWheelId)
	{
		CEventMouseWheel* mouseEvent=(CEventMouseWheel*)&event;
		MouseWheel += (mouseEvent->Direction? -1 : +1);
	}

	

}

void CGuiMouseListener::addToServer (CEventServer& server)
{
	server.addListener (EventMouseMoveId, this);
	server.addListener (EventMouseDownId, this);
	server.addListener (EventMouseUpId, this);
	server.addListener (EventMouseWheelId, this);
	_AsyncListener.addToServer (server);
}

void CGuiMouseListener::removeFromServer (CEventServer& server)
{
	server.removeListener (EventMouseMoveId, this);
	server.removeListener (EventMouseDownId, this);
	server.removeListener (EventMouseUpId, this);
	server.removeListener (EventMouseWheelId, this);
	_AsyncListener.removeFromServer (server);
}


CVector CGuiMouseListener::position()
{
	CVector res(0,0,0);
	res.x = MouseX * C3DTask::getInstance().screenWidth();
	res.y = MouseY * C3DTask::getInstance().screenHeight(); 
	return res;
}

CVector CGuiMouseListener::pressedPosition()
{
	CVector res(0,0,0);
	res.x = PressedX * C3DTask::getInstance().screenWidth();
	res.y = PressedY * C3DTask::getInstance().screenHeight(); 
	return res;
}

