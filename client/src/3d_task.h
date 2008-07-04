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

#ifndef MT_3D_TASK_H
#define MT_3D_TASK_H


//
// Includes
//

#include <nel/misc/singleton.h>

#include <nel/3d/u_scene.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>

#include "mouse_listener.h"


//
// Variables
//

extern NLMISC::CVariable<bool> Shadow;
//extern bool FastExit;

//
// Functions
//

void exitFunction();

//
// Classes
//

class C3DTask : public NLMISC::CSingleton<C3DTask>, public ITask
{
public:

	C3DTask() : Driver(0), Scene(0), CollisionManager(0), MouseListener(0) { }
	
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual string name() const { return "C3DTask"; }

	bool				isDriverAvailable() { return Driver != NULL; }

	NL3D::UDriver		&driver() const { if(!Driver) throw NLMISC::Exception("NoDriver"); return *Driver; }

	NL3D::UMaterial		createMaterial() const;

	NL3D::UScene		&scene() const { if(!Scene) throw NLMISC::Exception("NoScene"); return *Scene; }
	NL3D::UVisualCollisionManager	&collisionManager() const { if(!CollisionManager) throw NLMISC::Exception("NoCollisionManager"); return *CollisionManager; }
	C3dMouseListener	&mouseListener() const { if(!MouseListener) throw NLMISC::Exception("NoMouseListener"); return *MouseListener; }

	uint16				screenWidth() const { return ScreenWidth; }
	uint16				screenHeight() const { return ScreenHeight; }
	bool				fullscreen() const { return Fullscreen; }

	bool				kbPressed(NLMISC::TKey key) const;
	bool				kbDown(NLMISC::TKey key) const;
	ucstring			kbGetString() const;

	// only used by the IME to simulate a new character and put it in the kbGetString buffer
	void				kbAddToString(const ucstring &str);

	void				clear();
		
	void				captureCursor(bool b);

	void				clearColor(NLMISC::CRGBA color) { ClearColor = color; }
	void				takeScreenShot();

private:

	NL3D::UDriver			*Driver;
	NL3D::UScene			*Scene;
	NL3D::UVisualCollisionManager *CollisionManager;

	uint16					 ScreenWidth;
	uint16					 ScreenHeight;
	bool					 Fullscreen;

	NLMISC::CRGBA			 ClearColor;
	C3dMouseListener		*MouseListener;
	//bool					 CaptureCursor;
};

#endif
