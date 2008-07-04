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

#ifndef MT_TASK_H
#define MT_TASK_H


//
// Functions
//

#include <nel/misc/hierarchical_timer.h>


//
// Class
//

class ITask
{
public:

	ITask()
	{
		NeedRemove = false;
		Execute = true;
	}

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void release() = 0;
	virtual string name() const = 0;

	virtual void stop()
	{
		//nlinfo("Task %s is now stopped", name().c_str());
		Execute = false;
	}

	virtual void restart()
	{
		nlinfo("Task %s is restarted", name().c_str());
		Execute = true;
	}

	virtual void remove()
	{
		nlinfo("Task %s need to be remove", name().c_str());
		NeedRemove = true;
		Execute = false;
	}

	virtual bool execute() const { return Execute; }

private:

	sint32	Order;

	NLMISC::CHTimer	HTimerUpdate;
	NLMISC::CHTimer	HTimerRender;
	char NameUpdate[256];
	char NameRender[256];

	bool NeedRemove;
	bool Execute;

	friend class CTaskManager;
	friend struct displayTasksClass;
};


#endif
