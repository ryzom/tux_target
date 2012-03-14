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

#ifndef MTPT_LEVEL_MANAGER
#define MTPT_LEVEL_MANAGER

//
// Extern
//


//
// Includes
//

#include "level.h"


//
// Classes
//

class CLevelManager : public NLMISC::CSingleton<CLevelManager>, public ITask
{
public:

	CLevel &currentLevel() { nlassert(CurrentLevel); return *CurrentLevel; }

	bool levelPresent() {return CurrentLevel!=0;	}

	virtual void init();
	virtual void update() { }
	virtual void render() { }
	virtual void release();

	virtual std::string name() const { return "CLevelManager"; }
	
	void display(NLMISC::CLog *log = NLMISC::InfoLog) const;

	// find and load a new level
	void loadLevel(const std::string &fileName);
protected:
	friend class NLMISC::CSingleton<CLevelManager>;
	CLevelManager();

private:

	CLevel						*CurrentLevel;
};

#endif
