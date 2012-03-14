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
// This is the main class that manages all other classes
//

#ifndef MTPT_HUD_TASK_H
#define MTPT_HUD_TASK_H


//
// Includes
//
#include "time_task.h"


//
// Classes
//

class CHudMessage
{
public:
	CHudMessage(float x,float y,float scale,const std::string &message,NLMISC::CRGBA col,double duration)
	{
		this->x = x;
		this->y = y;
		this->scale = scale;
		this->message = message;
		this->col = col;
		if(duration==0)
			this->endTime = 0;
		else
			this->endTime = duration + CTimeTask::getInstance().time();
	}
	virtual ~CHudMessage()
	{

	}

	float x,y;
	float scale;
	NLMISC::CRGBA col;
	double endTime;
	std::string message;
protected:
private:
};

class CHudTask : public NLMISC::CSingleton<CHudTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual std::string name() const { return "CHudTask"; }

	void setDisplayViewedName(const std::string &name);
	void addSysMessage(const std::string &txt);
	void addMessage(const CHudMessage &message);

	std::list<CHudMessage> messages;
private:
	std::string _viewedName;
	
	float AltimeterMinValue;
	float AltimeterMaxValue;
	float AltimeterValue;

	bool pressControlMessageAdded;
	bool landClosedMessageAdded;
	bool landClosedMessageAdded2;
	
};

#endif
