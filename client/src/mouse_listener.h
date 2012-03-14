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
// This class manages the mouse input
//

#ifndef MOUSE_LISTENER_H
#define MOUSE_LISTENER_H


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
 * C3dMouseListener is a listener that handle a 3d matrix with mouse events.
 * This can be the view matrix, or the matrix of any object.
 */
class C3dMouseListener : public NLMISC::IEventListener
{
public:
	/** 
	  * Constructor. 
	  */
	C3dMouseListener();

	virtual ~C3dMouseListener();

	void init ();

	void reset();
		
	/** 
	  * Register the listener to the server.
	  */
	void addToServer (NLMISC::CEventServer& server);

	/** 
	  * Unregister the listener to the server.
	  */
	void removeFromServer (NLMISC::CEventServer& server);

	bool	InvertedMouse;

	float	MouseX, MouseY;
	sint	MouseWheel;

private:
	/// Internal use
	virtual void operator ()(const NLMISC::CEvent& event);
private:

	uint32 _allowMouse;
	bool _inverseMouse;

	NLMISC::CEventListenerAsync	_AsyncListener;
};


//
// Functions
//

void	initMouseListenerConfig();

#endif
