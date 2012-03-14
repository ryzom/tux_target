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

#include "command.h"

using namespace std;
using namespace NLNET;
using namespace NLMISC;


CCommand::CCommand(const char *categoryName, const char *commandName, const char *commandHelp, const char *commandArgs) : ICommand(categoryName, commandName, commandHelp, commandArgs)
{
	
}

void CCommand::execute (CEntity *entity, const std::string &commandWithArgs, CLog &log, bool quiet, bool human)
{
	if (!quiet)
	{
		ucstring temp;
		temp.fromUtf8(commandWithArgs);
		string disp = temp.toString();
		log.displayNL ("Executing command : '%s'", disp.c_str());
	}

	// convert the buffer into string vector
	vector<pair<string, vector<string> > > commands;
	
	bool firstArg = true;
	uint i = 0;
	while (true)
	{
		// skip whitespace
		while (true)
		{
			if (i == commandWithArgs.size())
			{
				goto end;
			}
			if (commandWithArgs[i] != ' ' && commandWithArgs[i] != '\t' && commandWithArgs[i] != '\n' && commandWithArgs[i] != '\r')
			{
				break;
			}
			i++;
		}
		
		// get param
		string arg;
		if (commandWithArgs[i] == '\"')
		{
			// starting with a quote "
			i++;
			while (true)
			{
				if (i == commandWithArgs.size())
				{
					if (!quiet) log.displayNL ("Missing end quote character \"");
					return;
				}
				if (commandWithArgs[i] == '"')
				{
					i++;
					break;
				}
				if (commandWithArgs[i] == '\\')
				{
					// manage escape char backslash
					i++;
					if (i == commandWithArgs.size())
					{
						if (!quiet) log.displayNL ("Missing character after the backslash \\ character");
						return;
					}
					switch (commandWithArgs[i])
					{
						case '\\':	arg += '\\'; break; // double backslash
						case 'n':	arg += '\n'; break; // new line
						case '"':	arg += '"'; break; // "
						default:
							if (!quiet) log.displayNL ("Unknown escape code '\\%c'", commandWithArgs[i]);
							return;
					}
					i++;
				}
				else
				{
					arg += commandWithArgs[i++];
				}
			}
		}
		else
		{
			// normal word
			while (true)
			{
				if (commandWithArgs[i] == '\\')
				{
					// manage escape char backslash
					i++;
					if (i == commandWithArgs.size())
					{
						if (!quiet) log.displayNL ("Missing character after the backslash \\ character");
						return;
					}
					switch (commandWithArgs[i])
					{
						case '\\':	arg += '\\'; break; // double backslash
						case 'n':	arg += '\n'; break; // new line
						case '"':	arg += '"'; break; // "
						case ';':	arg += ';'; break; // ;
						default:
							if (!quiet) log.displayNL ("Unknown escape code '\\%c'", commandWithArgs[i]);
							return;
					}
				}
				else if (commandWithArgs[i] == ';')
				{
					// command separator
					break;
				}
				else
				{
					arg += commandWithArgs[i];
				}

				i++;

				if (i == commandWithArgs.size() || commandWithArgs[i] == ' ' || commandWithArgs[i] == '\t' || commandWithArgs[i] == '\n' || commandWithArgs[i] == '\r')
				{
					break;
				}
			}
		}

		if (!arg.empty())
		{
			if (firstArg)
			{
				commands.push_back (make_pair(arg, vector<string> () ));
				firstArg = false;
			}
			else
			{
				commands[commands.size()-1].second.push_back (arg);
			}
		}

		// separator
		if (i < commandWithArgs.size() && commandWithArgs[i] == ';')
		{
			firstArg = true;
			i++;
		}
	}
end:

// displays args for debug purpose
/*	for (uint u = 0; u < commands.size (); u++)
	{
		nlinfo ("c '%s'", commands[u].first.c_str());
		for (uint t = 0; t < commands[u].second.size (); t++)
		{
			nlinfo ("p%d '%s'", t, commands[u].second[t].c_str());
		}
	}
*/

	for (uint u = 0; u < commands.size (); u++)
	{
		if(!CCommandRegistry::getInstance().isCommand(commands[u].first))
		{
			// the command doesn't exist
			if (!quiet) log.displayNL("Command '%s' not found, try 'help'", commands[u].first.c_str());
		}
		else
		{
			ICommand *icom = CCommandRegistry::getInstance().getCommand(commands[u].first);
			//ICommand *icom = (*comm).second;
			if(icom->CategoryName=="mtpt_commands")//TODO find a better way, bad macro usage can result in bad cast :(((
			{
				CCommand *ccom = (CCommand *)icom;
				if (!ccom->execute (entity,commands[u].second, log, quiet, human))
				{
					if (!quiet) log.displayNL("Bad ccommand usage, try 'help %s'", commands[u].first.c_str());
				}
			}
			else
			{
				if (!icom->execute(NULL, commands[u].second, log, quiet, human))
				{
					if (!quiet) log.displayNL("Bad icommand usage, try 'help %s'", commands[u].first.c_str());
				}
			}
		}
	}
}

