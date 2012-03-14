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
// This class manages all entities using a 256 entries static array.
// Using that static array to be able to be multithread safe.
// The entry 255 is always empty because 255 is the code for "unknown" entity.
//

#ifndef MTPT_TASK_MANAGER
#define MTPT_TASK_MANAGER


//
// Includes
//

#include <nel/misc/singleton.h>


//
// Classes
//

class CTaskManager : public NLMISC::CSingleton<CTaskManager>
{
public:

	// do all the init and execution
	void execute();

	// add a task into the manager
	void add(ITask &task, sint32 order = -1, bool startNow = true);

	// stop all tasks
	void stopAll();

	// remove the task from the manager
	void remove(ITask &task);

	// call this function if you want to exit the game
	void exit();

	// call this to enable/disable the benchmark
	void switchBench();

	CTaskManager();

protected:
	std::list<ITask*>	Tasks;
	std::list<ITask*>	OrderSortedTasks;

	bool Exit;

	bool Benching;

	friend struct commands_displayTasksClass;
};

#endif
