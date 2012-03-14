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
// This class manage the physics thread and every linked to ODE
//

#ifndef MTPT_PHYSICS
#define MTPT_PHYSICS


//
// Includes
//

#include <ode/ode.h>

#include <nel/misc/thread.h>
#include <nel/misc/vector.h>
#include <nel/misc/command.h>

#include <nel/net/service.h>


//
// Constants
//

#define MT_DEFAULT_SPECTRUM 0xffffffff
#define MT_SCENE_SPECTRUM 0x0001
#define MT_CLIENT_SPECTRUM 0x0002


//
// Functions
//

bool pausePhysics(bool waitAck=true);
bool isPhysicsPaused();
void resumePhysics();

void initPhysics();
void updatePhysics();
void releasePhysics();


//
// Classes
//

//
// Typedefs
//

typedef NLMISC::CSynchronized<NLMISC::TTime> SyncPhyTime;

//
// Variables
//

extern NLMISC::TTime syncStartPhyTime;
extern SyncPhyTime syncPhyTime;

extern dWorldID			World;

extern NLMISC::CSynchronized<dSpaceID>		Space;

extern dJointGroupID	ContactGroup;
extern dGeomID			Water;

#endif
