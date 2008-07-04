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

#ifndef MT_CONTROLER_H
#define MT_CONTROLER_H


//
// Includes
//

#include <nel/misc/vector.h>

#include "camera.h"
#include "entity.h"


//
// Classes
//

class CControler
{
public:
	CControler();
	~CControler();

	void	swapOpenClose();
	void	setControledEntity(uint8 eid);
	uint8	getControledEntity() const;
	void	update();
	void	reset();

	CCamera			Camera;

	uint8			ViewedEId;	// the id of the entity that we currently view (followed)

private:

	uint8			EId;

//	NLMISC::CVector Accel;

	float			Pique;
	float			RotZ;

	float			VirtualFrameTime;
	bool			FirstFrame;

//	float			MissedTime;

	void updateDebugKeys();
	void updateUserKeys();
};

#endif
