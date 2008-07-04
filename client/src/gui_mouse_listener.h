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

#ifndef MT_GUI_MOUSE_LISTENER_H
#define MT_GUI_MOUSE_LISTENER_H


//
// Includes
//

#include <nel/misc/types_nl.h>
#include <nel/misc/event_listener.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/time_nl.h>

#include <nel/3d/u_camera.h>
#include <nel/3d/viewport.h>
#include <nel/3d/frustum.h>
#include <nel/3d/u_3d_mouse_listener.h>


//
// Classes
//

/**
 * CGuiMouseListener is a listener that handle a 3d matrix with mouse events.
 * This can be the view matrix, or the matrix of any object.
 */
class CGuiMouseListener : public NLMISC::IEventListener
{
public:
	/** 
	  * Constructor. 
	  */
	CGuiMouseListener();

	virtual ~CGuiMouseListener();

	void init ();
	void update();

	/** 
	  * Register the listener to the server.
	  */
	void addToServer (NLMISC::CEventServer& server);

	/** 
	  * Unregister the listener to the server.
	  */
	void removeFromServer (NLMISC::CEventServer& server);

	void captureCursor(bool b) {_captureCursor = b;};
	
	bool	ButtonDown;
	bool	RightButtonDown;
	bool	Pressed;
	bool	Clicked;
	double  LastClickedTime;
	bool	DoubleClicked;
	float	MouseX, MouseY;
	float	PressedX, PressedY;
	sint	MouseWheel;

	NLMISC::CVector position();
	NLMISC::CVector pressedPosition();
	
private:
	/// Internal use
	virtual void operator ()(const NLMISC::CEvent& event);
private:
	bool    LastButtonDown;   
	bool    _captureCursor;
	bool    _cursorCaptured;
	
	NLMISC::CEventListenerAsync	_AsyncListener;
};


#endif
