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
#include <nel/3d/u_driver.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/event_server.h>
#include <nel/3d/u_visual_collision_entity.h>

#include <nel/misc/quat.h>
#include <nel/misc/plane.h>
#include <nel/misc/time_nl.h>

#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "gui_object.h"
#include "config_file_task.h"
#include "gui_mouse_listener.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


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
	LastClickedTime = CTimeTask::instance().time();
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
	double dtime = CTimeTask::instance().time()-LastClickedTime ;
	DoubleClicked = Clicked && dtime<0.3f;
	if(Clicked)
		LastClickedTime = CTimeTask::instance().time();
	
	LastButtonDown = ButtonDown;
	
	bool shouldCaptureCursor = CGuiObjectManager::instance().empty() && _captureCursor;
	if(shouldCaptureCursor != _cursorCaptured)
	{
		if(shouldCaptureCursor)
		{
			//C3DTask::instance().driver().setCapture(true);
			//C3DTask::instance().driver().showCursor(false);
			C3DTask::instance().mouseListener().addToServer(C3DTask::instance().driver().EventServer);
		}
		else
		{
			//C3DTask::instance().driver().setCapture(false);
			//C3DTask::instance().driver().showCursor(true);
			C3DTask::instance().mouseListener().removeFromServer(C3DTask::instance().driver().EventServer);
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
		CEventMouseWheel* mew=(CEventMouseWheel*)&event;
		MouseWheel += (mew->Direction? -1 : +1);
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
	res.x = MouseX * C3DTask::instance().screenWidth();
	res.y = MouseY * C3DTask::instance().screenHeight(); 
	return res;
}

CVector CGuiMouseListener::pressedPosition()
{
	CVector res(0,0,0);
	res.x = PressedX * C3DTask::instance().screenWidth();
	res.y = PressedY * C3DTask::instance().screenHeight(); 
	return res;
}
