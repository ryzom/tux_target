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

#include "graph.h"
#include "3d_task.h"
#include "time_task.h"


//
// Namespaces
//

using namespace NLMISC;


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
	RealDeltaTime = 0;
	FirstUpdate = true;
	TimeSpeed = 1.0;
}

void CTimeTask::speedTime(double dt)
{
	TimeSpeed = dt;
}

double CTimeTask::getSpeedTime() const
{
	return TimeSpeed;
}

void CTimeTask::update()
{
	// NEVER use getPerformanceTime because rdtsc on Linux can do bad things
	// like rdtsc second is not the same duration as getLocalTime duration
	// so it generates problems with LCT
	//double newTime = CTime::ticksToSecond(CTime::getPerformanceTime());
	double newTime = double(CTime::getLocalTime())/1000.0;

	if(FirstUpdate)
		FirstTime = newTime - 1; //-1 for replay 

	//Time = newTime - FirstTime;

	if(FirstUpdate)
	{
		DeltaTime = 0;
		RealDeltaTime = 0;
Time = 0;
		FirstUpdate = false;
	}
	else
	{
		RealDeltaTime = (newTime - OldTime);
		DeltaTime = RealDeltaTime * TimeSpeed;
Time += DeltaTime;
	}

	OldTime = newTime;

	DeltaTimeSmooth.addValue(RealDeltaTime);

	FpsGraph.addValue (1.0f);
	MSpfGraph.addOneValue (float(RealDeltaTime*1000.f));
}
