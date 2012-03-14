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
// This is the main class that manages all other classes
//

#ifndef MTPT_3D_TASK_H
#define MTPT_3D_TASK_H

//
// Includes
//
#include <nel/misc/singleton.h>

#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_texture.h>

#include "mouse_listener.h"


//
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

	virtual std::string name() const { return "C3DTask"; }
	
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
	std::string			kbGetString() const;

	void				clear();
		
	void				captureCursor(bool b);
	NL3D::UInstance     levelParticle()  { return LevelParticle;};
	void				clearColor(NLMISC::CRGBA color) { ClearColor = color; }
	void				takeScreenShot();

	bool EnableExternalCamera;

private:

	NL3D::UDriver			*Driver;
	NL3D::UScene			*Scene;
	NL3D::UVisualCollisionManager *CollisionManager;

	uint16					 ScreenWidth;
	uint16					 ScreenHeight;
	NLMISC::CRGBA			 AmbientColor;
	bool					 Fullscreen;
	NLMISC::CRGBA			 ClearColor;
	C3dMouseListener		*MouseListener;
	NL3D::UParticleSystemInstance LevelParticle;
	//bool					 CaptureCursor;
};

#endif
