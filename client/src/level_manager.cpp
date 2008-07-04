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

#include <nel/misc/path.h>
#include <nel/net/service.h>

#include "level.h"
#include "mtp_target.h"
#include "entity_manager.h"
#include "network_task.h"
#include "config_file_task.h"
#include "../../common/net_message.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NLNET;


//
// Functions
//

CLevelManager::CLevelManager() : NLMISC::CSingleton<CLevelManager>(), CurrentLevel(0)
{
}

void CLevelManager::init()
{
	CurrentLevel = 0;
}

void CLevelManager::update()
{
	if(CurrentLevel)
	{
		CurrentLevel->update();
	}
}

void CLevelManager::release()
{
	if(CurrentLevel)
	{
		delete CurrentLevel;
		CurrentLevel = 0;
	}
}

void CLevelManager::loadLevel(const string &fileName, bool useLookup)
{
	nlinfo("LEVEL: Loading level '%s'", fileName.c_str());
	if(CurrentLevel!=0)// || CurrentLevel->filename()!=res)
	{
		delete CurrentLevel;
		CurrentLevel = 0;
	}

	string res;
	if(useLookup) res = CPath::lookup(fileName, false);
	else res = fileName;
	if(res.empty())
	{
		nlwarning("LEVEL: level not found");

		// request the level from the server
		CNetMessage msgout(CNetMessage::Level);
		CNetworkTask::instance().send(msgout);
		return;
	}
	//if(CMtpTarget::instance().error()) return;
	//nlassert(!res.empty());

	CurrentLevel = new CLevel(res);
	if(!CurrentLevel->valid())
	{
		nlwarning("LEVEL: level not valid");
		return;
	}
}

void CLevelManager::display(CLog *log) const
{
	if(CurrentLevel)
		CurrentLevel->display(log);
	else
		log->displayNL("There's not current level");
}


//
// Commands
//

NLMISC_COMMAND(displayLevel, "display the current level", "")
{
	if(args.size() != 0) return false;
	CLevelManager::instance().display(&log);
	return true;
}
