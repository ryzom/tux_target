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


//
// Includes
//

#include "stdpch.h"

#include <nel/misc/command.h>

#include "main.h"
#include "task_manager.h"
#include "entity_manager.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//

void CTaskManager::switchBench()
{
	Benching = !Benching;
	if(Benching)
	{
		nlinfo("Starting benchmark...");
		CHTimer::clear();
	}
	else
	{
		nlinfo("End of benchmark");
		//CHTimer::display();
		//CHTimer::displayHierarchicalByExecutionPathSorted(InfoLog, CHTimer::TotalTime, true, 64);
		CHTimer::display(InfoLog, CHTimer::MaxSession);
		CHTimer::displayHierarchicalByExecutionPathSorted(InfoLog, CHTimer::MaxSession, true, 64);
	}
}

void CTaskManager::execute()
{
	Exit = false;
	Benching = false;

	//
	// Init the hierarchical timer
	//

	CHTimer::startBench();
	CHTimer::endBench();

	while(!Exit)
	{
		if(Benching) CHTimer::startBench(false, false, false);
		
//		nlinfo("***** new frame, update");
		for(list<ITask*>::iterator it = OrderSortedTasks.begin(); it != OrderSortedTasks.end(); it++)
		{
			ITask *task = *it;
			if((*it)->Execute)
			{
//				nlinfo("Update %s", (*it)->name().c_str());
				(*it)->HTimerUpdate.before();
				(*it)->update();
				(*it)->HTimerUpdate.after();
			}
		}

//		nlinfo("----- render");
		for(list<ITask*>::iterator it = OrderSortedTasks.begin(); it != OrderSortedTasks.end(); it++)
		{
			if((*it)->Execute)
			{
//				nlinfo("Render %s", (*it)->name().c_str());
				(*it)->HTimerRender.before();
				(*it)->render();
				(*it)->HTimerRender.after();
			}
		}
//		nlinfo("----- end frame");

		// release and remove task from Task
		for(list<ITask*>::iterator it2 = Tasks.begin(); it2 != Tasks.end(); )
		{
			if((*it2)->NeedRemove)
			{
				remove(*(*it2));
				it2 = Tasks.begin();
			}
			else
			{
				it2++;
			}
		}

		CHTimer::endBench();
	}

	for(list<ITask*>::reverse_iterator it = Tasks.rbegin(); it != Tasks.rend(); it++)
	{
		//nlinfo("Release %s", (*it)->name().c_str());
		(*it)->release();
	}
	Tasks.clear();
	OrderSortedTasks.clear();
}

void CTaskManager::remove(ITask &task)
{
	//nlinfo("Removing task %s", task.name().c_str());
	// release and remove task from Task
	for(list<ITask*>::iterator it = Tasks.begin(); it != Tasks.end();)
	{
		if(&task == (*it))
		{
			Tasks.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}
	// remove task from OrderSortedTasks
	for(list<ITask*>::iterator it = OrderSortedTasks.begin(); it != OrderSortedTasks.end();)
	{
		if(&task == (*it))
		{
			OrderSortedTasks.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}
	//nlinfo("Removed task %s from list", task.name().c_str());
	task.release();
	//nlinfo("Removed task %s and release called", task.name().c_str());
}

void CTaskManager::stopAll()
{
	nlinfo("Stopping all tasks");
	for(list<ITask*>::iterator it = Tasks.begin(); it != Tasks.end(); it++)
	{
		(*it)->stop();
	}
}

void CTaskManager::add(ITask &task, sint32 order, bool startNow)
{
// Check that the task is not already in the array
	list<ITask*>::iterator result = find(Tasks.begin(), Tasks.end(), &task);
	nlassert(result == Tasks.end());
	result = find(OrderSortedTasks.begin(), OrderSortedTasks.end(), &task);
	nlassert(result == OrderSortedTasks.end());

	task.Order = order;

	string n = task.name() + "Update";
	strcpy(task.NameUpdate, n.c_str());
	n = task.name() + "Render";
	strcpy(task.NameRender, n.c_str());
	task.HTimerUpdate.setName(task.NameUpdate);
	task.HTimerRender.setName(task.NameRender);

	task.NeedRemove = false;
	task.Execute = startNow;

	Tasks.push_back(&task);

	if(order == -1)
	{
		OrderSortedTasks.push_back(&task);
	}
	else
	{
		list<ITask*>::iterator it;
		for(it = OrderSortedTasks.begin(); it != OrderSortedTasks.end(); it++)
		{
			if((*it)->Order > order)
			{
				break;
			}
		}
		OrderSortedTasks.insert(it, &task);
	}

//	nlinfo("Init during exec %s", task.name().c_str());
	task.init();
}

CTaskManager::CTaskManager()
{
}

void CTaskManager::exit()
{
	Exit = true;
}


//
// Commands
//

NLMISC_COMMAND(displayTasks, "display all task", "")
{
	if(args.size() != 0) return false;

	log.displayNL("There's %d tasks:", CTaskManager::instance().Tasks.size());
	log.displayNL("Init order call:");
	for(list<ITask*>::iterator it = CTaskManager::instance().Tasks.begin(); it != CTaskManager::instance().Tasks.end(); it++)
	{
		//log.displayNL("  %s %s", (*it)->name().c_str(), ((*it)->Execute?"Running":"Stop"));
	}
	log.displayNL("Update order call:");
	for(list<ITask*>::iterator it = CTaskManager::instance().OrderSortedTasks.begin(); it != CTaskManager::instance().OrderSortedTasks.end(); it++)
	{
		log.displayNL("  %s", (*it)->name().c_str());
		(*it)->render();
	}
	log.displayNL("Release order call:");
	for(list<ITask*>::reverse_iterator it = CTaskManager::instance().Tasks.rbegin(); it != CTaskManager::instance().Tasks.rend(); it++)
	{
		log.displayNL("  %s", (*it)->name().c_str());
		(*it)->release();
	}

	return true;
}
