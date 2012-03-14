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

#include "bot.h"
#include "main.h"
#include "network.h"
#include "variables.h"
#include "lua_engine.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "session_manager.h"
#include "ending_session_state.h"
#include "running_session_state.h"

//
// Namespaces
//

using namespace std;
using namespace NLNET;
using namespace NLMISC;


//
// Functions
//

void CRunningSessionState::update()
{
	TTime currentTime = CTime::getLocalTime();
	if(CSessionManager::getInstance().forceEnding() || ( currentTime > CSessionManager::getInstance().startTime()+(TTime)TimeBeforeCheck && CSessionManager::getInstance().editMode()==0 ) )
	{
	vector<string> chat;

	{
		CEntityManager::EntityConstIt it;

		bool everybodyStopped = true;

		if(currentTime > CSessionManager::getInstance().startTime()+(TTime)CLevelManager::getInstance().timeTimeout())
		{
			nlinfo("Session time out!!!");
		}
		else
		{
			for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
			{
				CEntity *c = *it;
				if(c->InGame)
				{
					float val = c->meanVelocity();
					
					if(val>MinVelBeforeEnd) //si le client bouge
					{
						everybodyStopped = false;
					}
					else //si il est arrete
					{
						if(c->ArrivalTime==0 && c->CurrentScore>0)
						{
							c->ArrivalTime =(float)(currentTime - CSessionManager::getInstance().startTime())/1000.0f;
							nlinfo("*** ARRIVAL TIME '%s' %f",c->name().c_str(),c->ArrivalTime);

							CNetMessage msgout(CNetMessage::TimeArrival);
							uint8 eid = c->id();
							msgout.serial(eid);
							msgout.serial(c->ArrivalTime);
							CNetwork::getInstance().send(msgout);
						}
					}
				}
			}
		}

		if(everybodyStopped)
			nlinfo("everybodyStopped");

		if(CSessionManager::getInstance().forceEnding() || everybodyStopped)
		{

			CLuaEngine::getInstance().levelEndSession();
			CLuaEngine::getInstance().release();
			CEntityManager::getInstance().checkAfkClient();//kick away client

			nlinfo("compute best tit");
			CEntityManager::EntityConstIt  bestit1 = CEntityManager::getInstance().entities().end();
			CEntityManager::EntityConstIt  bestit2 = CEntityManager::getInstance().entities().end();
			CEntityManager::EntityConstIt  bestit3 = CEntityManager::getInstance().entities().end();
			for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
			{
				if((*it)->CurrentScore > 0 && (*it)->ArrivalTime > 11)
				{
					if(bestit1 == CEntityManager::getInstance().entities().end() || (*bestit1)->ArrivalTime >(*it)->ArrivalTime)
					{
						bestit1 = it;
					}
					else if(bestit2 == CEntityManager::getInstance().entities().end() || (*bestit2)->ArrivalTime >(*it)->ArrivalTime)
					{
						bestit2 = it;
					}
					else if(bestit3 == CEntityManager::getInstance().entities().end() || (*bestit3)->ArrivalTime > (*it)->ArrivalTime)
					{
						bestit3 = it;
					}
				}
			}
			nlinfo("compute bonus time");
			if(CLevelManager::getInstance().bonusTime())
			{
				switch(CEntityManager::getInstance().entities().size())
				{
				case 0:
				case 1:
					break;
				case 2:
					if(bestit1 != CEntityManager::getInstance().entities().end()) (*bestit1)->CurrentScore +=  50;
					break;
				case 3:
					if(bestit1 != CEntityManager::getInstance().entities().end()) (*bestit1)->CurrentScore += 100;
					if(bestit2 != CEntityManager::getInstance().entities().end()) (*bestit2)->CurrentScore +=  50;
					break;
				default:
					if(bestit1 != CEntityManager::getInstance().entities().end()) (*bestit1)->CurrentScore += 150;
					if(bestit2 != CEntityManager::getInstance().entities().end()) (*bestit2)->CurrentScore += 100;
					if(bestit3 != CEntityManager::getInstance().entities().end()) (*bestit3)->CurrentScore +=  50;
					break;
				}
			}

			nlinfo("send CNetMessage::EndSession to clients");
			CNetMessage msgout(CNetMessage::EndSession);

			// prepare a message for login service to update the score on database
			CMessage msgout2("SU");

			string levelname = CLevelManager::getInstance().levelFilename();
			uint32 nbplayers = 0;
			// count players
			for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
				if((*it)->type() == CEntity::Client)
					nbplayers++;

			float sessionduration = (float)(currentTime - CSessionManager::getInstance().startTime())/1000.0f;

			msgout2.serial(levelname, nbplayers, sessionduration);

			for(it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
			{
				if((*it)->type() == CEntity::Client)
				{
					bool breakTime = false; // ace set to false
					string res = CLevelManager::getInstance().updateStats((*it)->name(), (*it)->CurrentScore, (*it)->ArrivalTime, breakTime);
					if(!res.empty())
					{
						chat.push_back(res);
						if(breakTime)
						{
							(*it)->CurrentScore += 1000;
						}
					}
					CClient *cl = dynamic_cast<CClient*>(*it);
					sint32 uid = cl->uid(), score = cl->CurrentScore;
					float duration = cl->ArrivalTime;
					msgout2.serial(uid, score, duration);
				}

				(*it)->Score +=(*it)->CurrentScore;

				// only save the score in the cfg if shardId = 0 (mean that on lan mode)
				if(IService::getInstance()->ConfigFile.getVar("ShardId").asInt() == 0)
				{
					CConfigFile::CVar &accounts = IService::getInstance()->ConfigFile.getVar("Accounts");
					for(uint i = 0; i < accounts.size(); i+=3)
					{
						if(accounts.asString(i) == (*it)->name())
						{
							accounts.setAsString(toString((*it)->Score), i+2);
							break;
						}
					}
				}

				uint8 eid = (*it)->id();
				msgout.serial(eid, (*it)->CurrentScore, (*it)->Score);

				dBodySetLinearVel((*it)->Body, 0.0f, 0.0f, 0.0f);
				dBodySetAngularVel((*it)->Body, 0.0f, 0.0f, 0.0f);
				(*it)->Force = CVector::Null;
			}

			nlinfo("save config file");
			// now save scores
			IService::getInstance()->ConfigFile.save();
			
			// send the message to all entities
			CNetwork::getInstance().send(msgout);

			// only push the score to database if there s some clients
			if(nbplayers != 0)
				CUnifiedNetwork::getInstance()->send("LS", msgout2);

			CEntityManager::getInstance().saveAllValidReplay();
			nlinfo("replay saved");
			changeState(CEndingSessionState::getInstance());
			nlinfo("reset CRunningSessionState variables");
			CSessionManager::getInstance().endTime(currentTime);
			CSessionManager::getInstance().forceEnding(false);
		}
	}
	
	// chat must be call outside the client accessor
	for(uint i = 0; i < chat.size(); i++)
		CNetwork::getInstance().sendChat(chat[i]);
	}
}
