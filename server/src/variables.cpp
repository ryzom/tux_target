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

#define MTPT_DEC

#include <nel/net/service.h>
#include <nel/misc/command.h>

#include "variables.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Macros
//

#define IMP_VAR(__name) \
IService::getInstance()->ConfigFile.setCallback(#__name, __name##getVar); \
__name##getVar(IService::getInstance()->ConfigFile.getVar(#__name)); \
__name##FromCfg=true;

//
// Variables
//

string LevelName, Author;
bool LevelNameFromCfg, AuthorFromCfg;


//
// Initialisation
//

void stringGetVar(NLMISC::CConfigFile::CVar &var)
{
	if(var.Name == "LevelName") LevelName = var.asString();
	else if(var.Name == "Author") Author = var.asString();
}

void initVariables()
{
	IMP_VAR(BotAccuracyOpen);
	IMP_VAR(BotAccuracyClose);
	IMP_VAR(ForcedClientCount);
	IMP_VAR(OpenZSpeed);
	IMP_VAR(OpenMinZSpeed);
	IMP_VAR(OpenMinHSpeed);
	IMP_VAR(OpenMinAngleToZSpeed);
	IMP_VAR(OpenAccelCoef);
	IMP_VAR(CloseAccelCoef);
	IMP_VAR(SphereDensity);
	IMP_VAR(Gravity);
	IMP_VAR(NbWaitingClients);
	IMP_VAR(TimeBeforeStart);
	IMP_VAR(TimeBeforeRestart);
	IMP_VAR(TimeBeforeCheck);
	IMP_VAR(MinVelBeforeEnd);
	IMP_VAR(DefaultMaxOpenClose);
	IMP_VAR(BounceWater);
	IMP_VAR(BounceVelWater);
	IMP_VAR(BounceClient);
	IMP_VAR(BounceVelClient);
	IMP_VAR(BounceScene);
	IMP_VAR(BounceVelScene);
	IMP_VAR(AngularDecreasing);
	IMP_VAR(NbMaxClients);
//	IMP_VAR(TimeTimeout);
	IMP_VAR(ModuleScore);
	IMP_VAR(ModuleBounce);
	IMP_VAR(ModuleAccel);
	IMP_VAR(ModuleFriction);

	IService::getInstance()->ConfigFile.setCallback("LevelName", stringGetVar);
	stringGetVar(IService::getInstance()->ConfigFile.getVar("LevelName"));
	LevelNameFromCfg = true;

	IService::getInstance()->ConfigFile.setCallback("Author", stringGetVar);
	stringGetVar(IService::getInstance()->ConfigFile.getVar("Author"));
	AuthorFromCfg = true;
}
