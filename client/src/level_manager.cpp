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

#include "level.h"
#include "mtp_target.h"
#include "level_manager.h"
#include "resource_manager2.h"
#include "entity_manager.h"
#include "config_file_task.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Types
//


//
// Declarations
//


//
// Variables
//

//
// Functions
//

CLevelManager::CLevelManager() : NLMISC::CSingleton<CLevelManager>()
{
	CurrentLevel = 0;
}

void CLevelManager::init()
{
	CurrentLevel = 0;
}

void CLevelManager::release()
{
	if(CurrentLevel)
	{
		delete CurrentLevel;
		CurrentLevel = 0;
	}
}

void CLevelManager::loadLevel(const std::string &fileName)
{
	nlinfo("Loading level '%s'", fileName.c_str());


	if(CurrentLevel!=0)// || CurrentLevel->filename()!=res)
	{
		delete CurrentLevel;
		CurrentLevel = 0;
	}

	string res = CResourceManager::getInstance().get(fileName);
	if(CMtpTarget::getInstance().error()) return;
	nlassert(!res.empty());
	
	CurrentLevel = new CLevel(res);
	if(!CurrentLevel->valid())
	{
		nlwarning("level not valid, trying to quit properly");
		return;
	}

	//nlassert(CurrentLevel->valid());
	
	if(CConfigFileTask::getInstance().configFile().getVar("ForceCRCCheck").asInt())
		CResourceManagerLan::getInstance().clearCrcCheckTimes();
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
	CLevelManager::getInstance().display(&log);
	return true;
}
