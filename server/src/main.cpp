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

#include <nel/misc/vector.h>
#include <nel/misc/time_nl.h>
#include <nel/misc/command.h>
#include <nel/misc/displayer.h>
#include <nel/misc/bit_mem_stream.h>
#include <nel/misc/hierarchical_timer.h>

#include <nel/net/service.h>
#include <nel/net/udp_sock.h>

#include "time.h"
#include "main.h"
#include "level.h"
#include "physics.h"
#include "welcome.h"
#include "network.h"
#include "variables.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "session_manager.h"
#include "lua_engine.h"
#include "../../common/constant.h"


//
// Namespaces
//

using namespace std;
using namespace NLNET;
using namespace NLMISC;


//
// Variables
//

uint32 UpdatePacketSize = 0;

bool ForceFinish = false;

TTime LastPingSent = 0;	// When we send all information of client (ping)

TTime LastSentUpdate = 0;
TTime NextSendUpdate = 0;

const float GSCALE = 0.01f;


uint MainThreadId = 0;
uint NetworkThreadId = 0;
uint PhysicThreadId = 0;

CSynchronized<PauseFlags> servicePauseFlags("servicePauseFlags");
void checkServicePaused();

//
// Classes
//

class CMtpTargetService : public NLNET::IService
{
public:

	void init()
	{
		{
			FILE *fp = fopen("connection.stat", "ab");
			if(fp)
			{
				char d[80];
				time_t ltime;
				time(&ltime);
				struct tm *today = localtime(&ltime);
				strftime(d, 80, "%Y %m %d %H %M %S", today);
				fprintf(fp, "%u %s # Server restarted\n", ltime, d);
				fclose(fp);
			}
		}	  

		MainThreadId = myGetThreadId();
#ifdef NL_OS_WINDOWS
		BOOL res;
		if(ConfigFile.exists("HighPriority") && ConfigFile.getVar("HighPriority").asInt()==1)
		{
			res = SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
			//res = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
			nlinfo("High priority %s",res==TRUE?"set":"not set");
		}
#endif

		CPath::addSearchPath(ConfigFile.getVar("UserTexture").asString(), false, true);
		CPath::remapExtension("dds", "tga", true);

		initVariables();

		srand(CTime::getSecondsSince1970());

		initPhysics();

		CEntityManager::getInstance().init();

		CNetwork::getInstance().init();

		CSessionManager::getInstance().init();

		//setUpdateTimeout(MT_NETWORK_MY_UPDATE_PERIODE_MS);

		initWelcome();

		nlinfo("Mtp Target User Init");
	}
	
	bool	update()
	{
		CNetwork::getInstance().update();

		CLuaEngine::getInstance().levelPreUpdate();

		// in mono thread
		updatePhysics();

		CEntityManager::getInstance().update();
		
		// Update the current session
		CSessionManager::getInstance().update();

		CLevelManager::getInstance().update();
		CLuaEngine::getInstance().levelPostUpdate();
		
		checkServicePaused();
		updateConnectedClients();

		//CNetwork::getInstance().sleep(MT_NETWORK_MY_UPDATE_PERIODE_MS);
		CNetwork::getInstance().sleep(1);

		return true;
	}

	void release()
	{
		CLevelManager::getInstance().release();
		CEntityManager::getInstance().release();
		CSessionManager::getInstance().release();

		releasePhysics();
	}
};
 
NLNET_SERVICE_MAIN(CMtpTargetService, "MTS", "mtp_target_service", 0, EmptyCallbackArray, "", "");

//
// Functions
//

uint myGetThreadId()
{
#ifdef NL_OS_UNIX
	return (int)pthread_self();
#else
	return getThreadId();
#endif
}

void checkServicePaused()
{
	{
		bool pause;
		{
			CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
			pause = acces.value().pauseCount>0;
			if(pause)
				acces.value().ackPaused = true;
		}
		while (pause) 
		{
			nlSleep(10);
			{
				CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
				pause = acces.value().pauseCount>0;
				if(pause)
					acces.value().ackPaused = true;
			}
		}
	}
	{
		CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
		acces.value().ackPaused = false;
	}	
}

bool pauseService(bool waitAck)
{
	bool pause;
	{
		CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
		pause = acces.value().pauseCount>0;
	}
	if(!pause) 
	{
		bool ackPaused;
		{
			CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
			ackPaused = false;
			acces.value().pauseCount++;
		}
		if(!waitAck) return true;
		while(!ackPaused)
		{
			nlSleep(10);
			{
				CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
				ackPaused = acces.value().ackPaused;
			}
		}
	}
	else
		return false;
	return true;
}

bool isServicePaused()
{
	bool ackPaused;
	{
		CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
		ackPaused = acces.value().ackPaused;
	}
	return ackPaused;
}

void resumeService()
{
	CSynchronized<PauseFlags>::CAccessor acces(&servicePauseFlags);
	if(acces.value().pauseCount>0) 
	{
		acces.value().pauseCount--;
		nlassert(acces.value().pauseCount>=0);
	}	
}

CSynchronized<PauseFlags> pauseAllFlags("pauseAll");

bool pauseAllThread()
{
	{
		CSynchronized<PauseFlags>::CAccessor acces(&pauseAllFlags);
		if(acces.value().pauseCount>0)
			return false;
		acces.value().pauseCount = 1;
	}
	bool allOk = true;
	allOk = allOk && pauseService(false);
	if(!allOk)
	{
		CSynchronized<PauseFlags>::CAccessor acces(&pauseAllFlags);
		acces.value().pauseCount = 0;
		return false;
	}
	allOk = allOk && pausePhysics(false);
	if(!allOk)
	{
		resumeService();
		CSynchronized<PauseFlags>::CAccessor acces(&pauseAllFlags);
		acces.value().pauseCount = 0;
		return false;
	}
	allOk = allOk && true;
	if(!allOk)
	{
		resumeService();
		resumePhysics();
		CSynchronized<PauseFlags>::CAccessor acces(&pauseAllFlags);
		acces.value().pauseCount = 0;
		return false;
	}

	uint i = 0;
	uint maxLoopCount = 100; //wait one second max
	for(i=0;i<maxLoopCount;i++)
	{
		int threadPausedCount = 0;
		if(isServicePaused())
			threadPausedCount++;
		if(isPhysicsPaused())
			threadPausedCount++;
		if(true)
			threadPausedCount++;
		if(threadPausedCount==2)
			break;
		nlSleep(10);
	}
	if(i>=maxLoopCount)
	{
		nlwarning("pauseAllThread() failed after %d tries during %d ms",maxLoopCount,maxLoopCount*10);
		uint tid = myGetThreadId();
		if(tid==MainThreadId)
			nlwarning("pauseAllThread() called(%d) in MainThreadId(%d)",tid,MainThreadId);
		if(tid==NetworkThreadId)
			nlwarning("pauseAllThread() called(%d) in NetworkThreadId(%d)",tid,NetworkThreadId);
		if(tid==PhysicThreadId)
			nlwarning("pauseAllThread() called(%d) in PhysicThreadId(%d)",tid,PhysicThreadId);
		nlwarning("isServicePaused()=%s",isServicePaused()?"true":"false");
		nlwarning("isPhysicsPaused()=%s",isPhysicsPaused()?"true":"false");
		nlwarning("resuming paused thread ...");
		resumeAllThread();
		return false;
	}
	return true;
}

void resumeAllThread()
{
	resumeService();
	resumePhysics();
	{
		CSynchronized<PauseFlags>::CAccessor acces(&pauseAllFlags);
		acces.value().pauseCount = 0;
	}
}

void reparsePath()
{
	CConfigFile::CVar *var = NULL;
	if ((var = IService::getInstance()->ConfigFile.getVarPtr ("Paths")) != NULL)
	{
		for (uint i = 0; i < var->size(); i++)
		{
			CPath::addSearchPath (var->asString(i), true, false);
		}
	}
	CPath::addSearchPath(IService::getInstance()->ConfigFile.getVar("UserTexture").asString(), false, true);
}


//
// Commands
//

NLMISC_COMMAND(broadcast, "send a message to everybody", "<string>")
{
	if(args.size() == 0)
		return false;
	
	string msg;
	
	for(uint i = 0; i < args.size(); i++)
		msg += args[i]+" ";
	
	CNetwork::getInstance().sendChat(msg);
	
	return true;
}

NLMISC_VARIABLE(uint32, UpdatePacketSize, "update packet size");
