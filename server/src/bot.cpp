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

#include <nel/misc/path.h>
#include <nel/net/service.h>

#include "bot.h"
#include "main.h"
#include "entity.h"
#include "physics.h"
#include "variables.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "session_manager.h"

//
// Namespaces
//

using namespace std;
using namespace NLNET;
using namespace NLMISC;


//
// Functions
//

CBot::CBot(uint8 eid, const string &name, bool isAutomaticBot) :
	CEntity(eid, findNewBotName(name)), IsAutomaticBot(isAutomaticBot)
{
	// no random color for bot, it s too ugly :) Color.set(rand()%255, rand()%255, rand()%255);
	Color.set(255, 255, 255);
	Ready = true;
}

CBot::~CBot()
{
}

void CBot::update()
{
	H_AUTO(CBotUpdate);

	while(!Commands.empty() && Commands.front().Time < ((CTime::getLocalTime() - CSessionManager::getInstance().startTime()) / 1000.0f))
	{
		//nlinfo("bot command = %d) ",Commands.front().Type);
		if(Commands.front().Type == CCommand::Position)
		{
			//nlinfo("bot update pos (eid = %d) %f %f %f",id(),Commands.front().Force.x,Commands.front().Force.y,Commands.front().Force.z);
			setForce(Commands.front().Force);
		}
		else if(Commands.front().Type == CCommand::OpenClose)
		{
			//nlinfo("bot update openclose (eid = %d) ",id());
			CEntityManager::getInstance().openClose(id());
			//openClose();
		}
		else
		{
			nlwarning("Unknown command for user %hu", id());
			nlstop;
		}
		Commands.pop_front();
	}
	CEntity::update();
}

void CBot::getBotDelta(NLMISC::CVector &delta)
{
	if(Commands.empty())
	{
		delta = CVector::Null;
		return;
	}
	
	delta = Commands.front().Pos - Pos;//CVector v(c->commands.front().posX - c->X,c->commands.front().posY - c->Y,c->commands.front().posZ - c->Z);
	if(delta.norm() < 0.001f)
	{
		delta = CVector::Null;
		return;
	}
	if(delta.norm() > 10.0f)
	{
		delta = CVector::Null;
		return;
	}
	
	delta *= OpenClose ? BotAccuracyOpen : BotAccuracyClose;
	if(delta.norm()>0.001f)
	{
		delta.normalize();
		delta *= 0.001f;
	}
	//nlinfo("id = %d , vnorm = %f = com(%f %f %f) - phy(%f %f %f)",id(),delta.norm(),Commands.front().Pos.x,Commands.front().Pos.y,Commands.front().Pos.z,Pos.x,Pos.y,Pos.z);	
}

void CBot::loadBotReplay()
{
	bool validFile = false;
	

	uint32 tryCount = 0;
	while (tryCount<10) //limit to 10 tries, prevent infinit loop and too long freez
	{
		tryCount++;
		vector<string> res;
		CPath::getPathContent("./replay", false, false, true, res);
		vector<string> levels;
		string level = "";
		string CurrentLevel = CLevelManager::getInstance().currentLevel().name();
		string wantedFilename = CurrentLevel + "." + toString(StartingPointId);
		for(uint i = 0; i < res.size(); i++)
		{
			if(res[i].find(wantedFilename) != string::npos)
				levels.push_back(res[i]);
			if(res[i].find(CurrentLevel) != string::npos)
				level = res[i];
		}

		Commands.clear();
					
		if(!levels.empty())
		{
			level = levels[rand()%levels.size()];
		}
		else
		{
			nlwarning("There's no replay matching start pos id %hu trying to use default one", (uint16)StartingPointId);
		}
		
		if(level.empty())
		{
			nlwarning("There's no replay file for the level",CurrentLevel.c_str());
			return;
		}

		nlinfo("Loading replay file (startposid = %d) : %s", StartingPointId, level.c_str());
		
		FILE *fp = fopen(level.c_str(),"rt");
		if(!fp)
		{
			nlwarning("Replay file '%s' can't be open", level.c_str());
			return;
		}
		
		char cmd[10]="deadbeef";
		float t;
		CVector force, pos;

		while(!feof(fp))
		{
			int eid;
			fscanf(fp, "%d %s %f", &eid, &cmd,  &t);
			if(string(cmd) == "PO")
			{
				fscanf(fp, "%f %f %f %f %f %f", &force.x, &force.y, &force.z, &pos.x, &pos.y, &pos.z);
				Commands.push_back(CCommand(t, force, pos));
			}
			else if(string(cmd) == "OC")
			{
				Commands.push_back(CCommand(t, pos));
			}
			else if(string(cmd) == "AU")
			{
				char name[255];
				sint score;
				uint32 argCount = 0;
				argCount = fscanf(fp, "%s %d", &name, &score);
				validFile = true;
				break;
			}
			else
			{
				nlwarning("Unknown command '%s' for user %hu", cmd, (uint16)eid);
				break;
			}
		}
		fclose(fp);
		if(!validFile) //invalid file format so delet it
		{
			CFile::deleteFile(level);
			nlwarning("deleting invalid replay (bad file format): %s",level.c_str());
		}
		else 
			return;
	}
	nlwarning("Give up finding a valid replay");
}

string CBot::findNewBotName(const string &name)
{
	if (!name.empty())
		return name;
	
	string res;
	CConfigFile::CVar &accounts = IService::getInstance()->ConfigFile.getVar("ForcedClientName");
	sint pos = rand() % accounts.size();
	for(uint i = pos; i < accounts.size(); i++)
	{
		if(!CEntityManager::getInstance().nameExist(accounts.asString(i)))
			return accounts.asString(i);
	}
	for(sint i = 0; i < pos; i++)
	{
		if(!CEntityManager::getInstance().nameExist(accounts.asString(i)))
			return accounts.asString(i);
	}
	return "albot";
}

void CBot::display(CLog &log)
{
	log.displayNL("  B eid %hu name %-10s score %3d/%4d ping %hu pos (%.2f,%.2f,%.2f)",
		(uint16)id(), name().c_str(), CurrentScore, Score, Ping.getSmoothValue(), Pos.x, Pos.y, Pos.z);
}


//
// Commands
//
