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

#ifndef MT_TIME_TASK_H
#define MT_TIME_TASK_H


//
// Includes
//

#include <nel/misc/singleton.h>
#include <nel/misc/value_smoother.h>


//
// Classes
//

class CTimeTask : public NLMISC::CSingleton<CTimeTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render() { }
	virtual void release() { }

	virtual void reset();

	virtual string name() const { return "CTimeTask"; }
	
	// Current time in seconds
	double		time() const { return Time; }

	// Delta time between last and this frame in seconds
	double		deltaTime () const { return DeltaTime; }
	
	// Delta time between last and this frame in seconds without the slowmotion
	double		realDeltaTime () const { return RealDeltaTime; }

	// Number of frame render in one second (average)
	double		fps() const { return DeltaTimeSmooth.getSmoothValue() ? 1.0 / DeltaTimeSmooth.getSmoothValue() : 0.0; }

	// Number of seconds to render a frame (average)
	double		spf() const { return DeltaTimeSmooth.getSmoothValue(); }

	void		speedTime(double st);	// 0.0 is stop 1.0 is normal 2.0 is two time faster
	double		getSpeedTime() const;

private:

	NLMISC::CValueSmootherTemplate<double> DeltaTimeSmooth;

	double	Time, OldTime;
	double	DeltaTime, RealDeltaTime;
	double	FirstTime;
	bool	FirstUpdate;
	double	TimeSpeed;
};

#endif
