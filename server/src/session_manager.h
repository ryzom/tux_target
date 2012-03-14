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

#ifndef MTPT_SESSION_MANAGER
#define MTPT_SESSION_MANAGER


//
// Includes
//

#include <nel/misc/singleton.h>

#include "session_state.h"
#include "entity_manager.h"


//
// Class
//

class CSessionManager : public NLMISC::CSingleton<CSessionManager>
{
public:
	
	void init();
	void update();
	void release();

	void changeState(CSessionState &ns);

	void forceEnding(bool b) { ForceEnding = b; }
	bool forceEnding() const { return ForceEnding; }

	void startTime(NLMISC::TTime st) { StartTime = st; }
	NLMISC::TTime startTime() const { return StartTime; }

	void endTime(NLMISC::TTime et) { EndTime = et; }
	NLMISC::TTime endTime() const { return EndTime; }
	
	std::string currentStateName() { nlassert(CurrentState); return CurrentState->name(); }

	void reset();
	
	uint8 editMode() {return _editMode;}
	void editMode(uint8 em) {_editMode = em;}
private:

	CSessionState *CurrentState;

	bool ForceEnding;

	NLMISC::TTime StartTime;
	NLMISC::TTime EndTime;

	uint8 _editMode;
};

#endif
