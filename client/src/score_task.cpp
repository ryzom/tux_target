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

#include "level.h"
#include "3d_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "mtp_target.h"
#include "score_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "background_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//
	
void CScoreTask::init()
{
}

void CScoreTask::update()
{
}

void CScoreTask::render()
{
	if(!C3DTask::instance().kbDown(KeyTAB) && CMtpTarget::instance().State != CMtpTarget::eEndSession)
		return;

	C3DTask::instance().driver().setFrustum(CFrustum(0, (float)C3DTask::instance().screenWidth(), 0, (float)C3DTask::instance().screenHeight(), -1, 1, false));
	C3DTask::instance().driver().clearZBuffer();

	C3DTask::instance().driver().drawQuad(0.0f, 0.0f, C3DTask::instance().screenWidth() , C3DTask::instance().screenHeight(), CRGBA (0, 0, 0, 200));

	// display all players score
	float fs = (float)CFontManager::instance().textContext(CFontManager::TCBig).getFontSize();
	float y = 150.0f/fs;
	float x1 = 10.0f/fs;

	if(CLevelManager::instance().levelPresent())
	{
		CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::MiddleBottom);
		CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 200, 0), CFontManager::instance().screenCenterX(), 5.0f, CLevelManager::instance().currentLevel().info());
		CFontManager::instance().textContext(CFontManager::TCBig).setHotSpot(UTextContext::BottomLeft);

		ucstring str = CI18N::get("Level") + ": " + CLevelManager::instance().currentLevel().name();
		if(CMtpTarget::instance().TeamMode) str += " " + CI18N::get("InTeam");
		str += " (" + CFile::getFilenameWithoutExtension(CLevelManager::instance().currentLevel().filename()) + ")";
		CFontManager::instance().print(CFontManager::TCBig, CRGBA(255, 255, 255), x1, y++, str);
	}

	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x1, y++, CMtpTarget::instance().String1);
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(253, 207, 85), x1, y++, CMtpTarget::instance().String2);

	float x2 = 450.0f/fs;
	float x3 = 560.0f/fs;
	float x4 = 660.0f/fs;
	float x5 = 720.0f/fs;
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x1, y, CI18N::get("Name"));
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x2, y, CI18N::get("Score"));
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x3, y, CI18N::get("TotalScore"));
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x4, y, CI18N::get("State"));
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x5, y, CI18N::get("Ping"));
	y += 1;

	vector<uint8> ids;
	CEntityManager::instance().getEIdSortedByScore(ids);

	for(uint i = 0; i < ids.size(); i++, y++)
	{
		CRGBA col(255,255,255);
		if(CEntityManager::instance()[ids[i]].isLocal())
			col = CRGBA(255,200,0);

		CFontManager::instance().print(CFontManager::TCBig, CEntityManager::instance()[ids[i]].color(), x1, y, CEntityManager::instance()[ids[i]].name());
		CFontManager::instance().print(CFontManager::TCBig, col, x2, y, ucstring(toString("%d", CEntityManager::instance()[ids[i]].lastGameScore())));
		CFontManager::instance().print(CFontManager::TCBig, col, x3, y, ucstring(toString("%d", CEntityManager::instance()[ids[i]].totalScore())));
		ucstring euro;
		euro += ucchar(0x20AC);
		CFontManager::instance().print(CFontManager::TCBig, col, x4, y, ucstring::makeFromUtf8(toString("%s %s %s", (CEntityManager::instance()[ids[i]].fullVersion()?euro.toUtf8().c_str():""), (CEntityManager::instance()[ids[i]].ready()?"":"W"), (CEntityManager::instance()[ids[i]].spectator()?"S":""))));
		CFontManager::instance().print(CFontManager::TCBig, col, x5, y, ucstring(toString("%u", CEntityManager::instance()[ids[i]].ping())));
	}
	CFontManager::instance().print(CFontManager::TCBig, CRGBA(245, 238, 141), x1, y++, CI18N::get("NumberOfPlayers")+toString(": %u", ids.size()));
}

void CScoreTask::release()
{
}
