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

#include "3d_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "score_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "entity_manager.h"
#include "background_task.h"
#include "resource_manager2.h"
#include "mtp_target.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//


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
	if(!C3DTask::getInstance().kbDown(KeyTAB) && CMtpTarget::getInstance().State != CMtpTarget::eEndSession )
		return;

	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	C3DTask::getInstance().driver().clearZBuffer();

	C3DTask::getInstance().driver().drawQuad(0.0f, 0.0f, C3DTask::getInstance().screenWidth() , C3DTask::getInstance().screenHeight(), CRGBA (0, 0, 0, 100));
	
	// display all players score
	float x1 = 15.0f;
	float x2 = 250.0f;
	float x3 = 380.0f;
	float x4 = 500.0f;
	float x5 = 680.0f;
	float y = 100.0f;
	CFontManager::getInstance().printf(CRGBA(245, 238, 141), x1, y, 1, "name");
	CFontManager::getInstance().printf(CRGBA(245, 238, 141), x2, y, 1, "score");
	CFontManager::getInstance().printf(CRGBA(245, 238, 141), x3, y, 1, "state");
	CFontManager::getInstance().printf(CRGBA(245, 238, 141), x4, y, 1, "total");
	CFontManager::getInstance().printf(CRGBA(245, 238, 141), x5, y, 1, "ping");
	y += CFontManager::getInstance().fontHeight()+10;
	
	vector<uint8> eids;
	CEntityManager::getInstance().getEIdSortedByScore(eids);
	
	for(uint i = 0; i < eids.size(); i++, y += CFontManager::getInstance().fontHeight())
	{
		sint32 curScore = CEntityManager::getInstance()[eids[i]].currentScore();
		if(curScore<0)
			printf("ok");
		CRGBA col(255,255,255,255);
		//col = CEntityManager::getInstance()[eids[i]].color();
		if(CEntityManager::getInstance()[eids[i]].isLocal())
			col = CRGBA(255,200,0,255);

		CFontManager::getInstance().printf(col, x1, y, 1, "%s", CEntityManager::getInstance()[eids[i]].name().c_str());
		CFontManager::getInstance().printf(col, x2, y, 1, "%d", CEntityManager::getInstance()[eids[i]].currentScore());
		CFontManager::getInstance().printf(col, x3, y, 1, "%s%s", (CEntityManager::getInstance()[eids[i]].ready()?"":"$"), (CEntityManager::getInstance()[eids[i]].spectator()?"@":""));
		CFontManager::getInstance().printf(col, x4, y, 1, "%d", CEntityManager::getInstance()[eids[i]].totalScore());
		CFontManager::getInstance().printf(col, x5, y, 1, "%u", CEntityManager::getInstance()[eids[i]].ping());
	}
}

void CScoreTask::release()
{
}
