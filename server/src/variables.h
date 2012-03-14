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

#ifndef MTPT_VARIABLES
#define MTPT_VARIABLES


//
// Includes
//

#include <nel/net/service.h>
#include <nel/misc/command.h>


//
// Macros
//

#ifdef MTPT_DEC

#define DEC_VAR(__name) \
bool __name##FromCfg; \
float __name; \
static void __name##getVar(NLMISC::CConfigFile::CVar &var) { __name = var.asFloat(); __name##FromCfg=true; } \
NLMISC_VARIABLE(float, __name, "")

#else // MTPT_DEC

#define DEC_VAR(__name) \
extern bool __name##FromCfg; \
extern float __name

#endif // MTPT_DEC


//
// Variables
//

DEC_VAR(BotAccuracyOpen);
DEC_VAR(BotAccuracyClose);
DEC_VAR(ForcedClientCount);
DEC_VAR(OpenZSpeed);
DEC_VAR(OpenMinZSpeed);
DEC_VAR(OpenMinHSpeed);
DEC_VAR(OpenMinAngleToZSpeed);
DEC_VAR(OpenAccelCoef);
DEC_VAR(CloseAccelCoef);
DEC_VAR(SphereDensity);
DEC_VAR(Gravity);
DEC_VAR(NbWaitingClients);
DEC_VAR(TimeBeforeStart);
DEC_VAR(TimeBeforeRestart);
DEC_VAR(TimeBeforeCheck);
DEC_VAR(MinVelBeforeEnd);
DEC_VAR(DefaultMaxOpenClose);
DEC_VAR(BounceWater);
DEC_VAR(BounceVelWater);
DEC_VAR(BounceClient);
DEC_VAR(BounceVelClient);
DEC_VAR(BounceScene);
DEC_VAR(BounceVelScene);
DEC_VAR(AngularDecreasing);
DEC_VAR(NbMaxClients);
//DEC_VAR(TimeTimeout);
DEC_VAR(ModuleScore);
DEC_VAR(ModuleBounce);
DEC_VAR(ModuleAccel);
DEC_VAR(ModuleFriction);

extern std::string LevelName, Author;
extern bool LevelNameFromCfg, AuthorFromCfg;

//
// Functions
//

void initVariables();

#endif
