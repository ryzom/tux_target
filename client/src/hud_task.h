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

#ifndef MT_HUD_TASK_H
#define MT_HUD_TASK_H


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
	// x == -1.0f means that we display the message in stacked manner with chat font
	// x == -2.0f means that we display in the center X of the screen
	CHudMessage(float x, float y, float scale, NLMISC::CRGBA col, double duration, const ucstring &message, NL3D::UTextContext::THotSpot hs = NL3D::UTextContext::BottomLeft)
	{
		this->x = x;
		this->y = y;
		this->scale = scale;
		this->message = message;
		this->col = col;
		if(duration==0)
			this->endTime = 0;
		else
			this->endTime = duration + CTimeTask::instance().time();
		this->hotSpot = hs;
	}
	virtual ~CHudMessage() { }

	float x, y;
	float scale;
	NLMISC::CRGBA col;
	double endTime;
	ucstring message;
	NL3D::UTextContext::THotSpot	hotSpot;
};

class CHudTask : public NLMISC::CSingleton<CHudTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release() { }

	virtual string name() const { return "CHudTask"; }

	void setDisplayViewedName(uint8 eid) { ViewedEId = eid; }

	void addMessage(const CHudMessage &message);

	void addSysMessage(const ucstring &txt);

	void clearMessages();

	list<CHudMessage> Messages;

private:

	void displayTutorial();

	uint8 ViewedEId;

	float AltimeterMinValue;
	float AltimeterMaxValue;
	float AltimeterValue;
};

#endif
