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

#ifndef MT_LEVEL_MANAGER_H
#define MT_LEVEL_MANAGER_H


//
// Includes
//

#include "../../common/lua_utility.h"


//
// Classes
//

class CLevel;

class CLevelManager : public NLMISC::CSingleton<CLevelManager>, public ITask
{
public:

	CLevel &currentLevel() { nlassert(CurrentLevel); return *CurrentLevel; }

	bool levelPresent() { return CurrentLevel!=0; }

	virtual void init();
	virtual void update();
	virtual void render() { }
	virtual void release();

	virtual string name() const { return "CLevelManager"; }
	
	void display(NLMISC::CLog *log = NLMISC::InfoLog) const;

	// find and load a new level
	void loadLevel(const string &fileName, bool useLookup = true);

protected:

	friend class NLMISC::CSingleton<CLevelManager>;
	CLevelManager();

private:

	CLevel						*CurrentLevel;
};

#endif
