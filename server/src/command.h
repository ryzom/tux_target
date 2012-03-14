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


#ifndef MTPT_COMMAND_H
#define MTPT_COMMAND_H


//
// Includes
//

#include "entity.h"

class CCommand;

//
// Helpers
//
#define MTPT_COMMAND(__name,__help,__args) MTPT_CATEGORISED_COMMAND(mtpt_commands,__name,__help,__args)
#define MTPT_CATEGORISED_COMMAND(__category,__name,__help,__args) \
struct __category##_##__name##Class: public CCommand \
{ \
	__category##_##__name##Class() : CCommand(#__category,#__name,__help,__args) { } \
	virtual bool execute(CEntity *entity, const std::vector<std::string> &args, NLMISC::CLog &log, bool quiet, bool human); \
	virtual bool execute(const std::string &rawCommandString, const std::vector<std::string> &args, NLMISC::CLog &log, bool quiet, bool human) { execute(NULL, args, log, quiet, human); } \
}; \
__category##_##__name##Class __category##_##__name##Instance; \
bool __category##_##__name##Class::execute(CEntity *entity, const std::vector<std::string> &args, NLMISC::CLog &log, bool quiet, bool human)

//
// Classes
//
class CCommand : public NLMISC::ICommand
{
public:

	CCommand(const char *categoryName, const char *commandName, const char *commandHelp, const char *commandArgs);

	virtual bool execute(CEntity *entity, const std::vector<std::string> &args, NLMISC::CLog &log, bool quiet, bool human = true)=0;

	virtual bool execute(const std::vector<std::string> &args, NLMISC::CLog &log, bool quiet, bool human = true)
	{
		//nlinfo("this command cannot be called from server");
		//return false;
		execute(NULL,args,log,quiet,human);
		return true;
	}

	static void execute (CEntity *entity, const std::string &commandWithArgs, NLMISC::CLog &log, bool quiet = false, bool human = true);

};

#endif
