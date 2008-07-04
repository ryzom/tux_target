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

#include <nel/misc/md5.h>

#include <nel/net/callback_client.h>

//#include "web.h"
#include "3d_task.h"
#include "end_task.h"
#include "gui_task.h"
#include "time_task.h"
#include "game_task.h"
#include "gui_check.h"
#include "mtp_target.h"
#include "login_client.h"
#include "font_manager.h"
#include "network_task.h"
#include "task_manager.h"
#include "background_task.h"
#include "config_file_task.h"


//
// Namespaces
//

using namespace NL3D;
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//


//
// Classes
//


//
// Functions
//

uint32 BeginTime, CurrentTime, TimeLeft;

void CEndTask::init()
{
	CBackgroundTask::instance().restart();
	BeginTime = CurrentTime = CTime::getSecondsSince1970();
}

void CEndTask::update()
{
	CurrentTime = CTime::getSecondsSince1970();
	TimeLeft = 10-(CurrentTime-BeginTime);
	if(TimeLeft == 0)
		CTaskManager::instance().exit();
}

void print(float x, float y, const ucstring &str)
{
	CFontManager::TextContext tc = CFontManager::TCBig;
	CViewport vp = C3DTask::instance().scene().getViewport();
	float onePixelX = 1.0f;
	float onePixelY = 1.0f;
	float fs2 = float(CFontManager::instance().textContext(tc).getFontSize())+5;
	float fs = float(CFontManager::instance().textContext(tc).getFontSize());
	CFontManager::instance().textContext(tc).setShaded(false);
	float x1 = (x*fs2+onePixelX)/fs;
	float y1 = (y*fs2+onePixelY)/fs;
	float x2 = (x*fs2-onePixelX)/fs;
	float y2 = (y*fs2-onePixelY)/fs;
	float x3 = (x*fs2)/fs;
	float y3 = (y*fs2)/fs;
	CFontManager::instance().print(tc, CRGBA(0,0,0), x1, y3, str, false);
	CFontManager::instance().print(tc, CRGBA(0,0,0), x2, y3, str, false);
	CFontManager::instance().print(tc, CRGBA(0,0,0), x3, y1, str, false);
	CFontManager::instance().print(tc, CRGBA(0,0,0), x3, y2, str, false);
	CFontManager::instance().print(tc, CRGBA(255,255,255), x3, y3, str, false);
}

void CEndTask::render()
{
	CFontManager::instance().print(CFontManager::TCDebug, 0, 0, "v" + ucstring(ReleaseVersion));
	float y = 15.0f;
	CRGBA col (255,255,255);
	print(7, y++, ucstring("Don't forget to buy the full version (15 euro) of Mtp Target to :"));
	print(10, y++, ucstring(" - Help developers to add new features and levels"));
	print(10, y++, ucstring(" - Play new levels"));
	print(10, y++, ucstring(" - Customize your avatar"));
	print(10, y++, ucstring(" - Remove all advertisement"));
	print(10, y++, ucstring(" - Quit the game immediately"));

	y+=2;

	print(10, y++, ucstring(toString("The game will close in %d seconds", TimeLeft)));
}
