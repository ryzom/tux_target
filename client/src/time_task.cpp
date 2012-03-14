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
// Includes
//

#include "stdpch.h"

#include "time_task.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	
void CTimeTask::init()
{
	reset();
}

void CTimeTask::reset()
{
	FirstTime = 1;
	OldTime = 1;
	Time = 1;
	DeltaTime = 0;
	FirstUpdate = true;
}

void CTimeTask::update()
{
	OldTime = Time;

	
	double newTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());

	if(FirstUpdate)
		FirstTime = newTime - 1; //-1 for replay 
	
	Time = newTime - FirstTime;

	if(FirstUpdate)
	{
		DeltaTime = 0;
		FirstUpdate = false;
	}
	else
		DeltaTime = Time - OldTime;
	
	DeltaTimeSmooth.addValue(DeltaTime);
}
