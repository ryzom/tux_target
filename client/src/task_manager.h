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

#ifndef MT_TASK_MANAGER_H
#define MT_TASK_MANAGER_H


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
	list<ITask*>	Tasks;
	list<ITask*>	OrderSortedTasks;

	bool Exit;

	bool Benching;

	friend struct commands_displayTasksClass;
};

#endif
