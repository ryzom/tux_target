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

#ifndef MTPT_TASK
#define MTPT_TASK


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
	virtual std::string name() const = 0;

	virtual void stop()
	{
		nlinfo("Task %s is now stopped", name().c_str());
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
