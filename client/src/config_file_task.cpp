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

#include <nel/misc/variable.h>

#include "config_file_task.h"
#include "userpaths.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	
void CConfigFileTask::init()
{
	const string ConfigFileName = "tux-target.cfg";
	string ConfigFilePath;

	// First, we look at the config file file in the current directory
	if(!CFile::fileExists(ConfigFilePath + ConfigFileName))
	{
#ifdef NL_OS_WINDOWS
		string DirName = "Tux Target";
#else
		string DirName = "tux-target";
#endif

		// Second, we look at the config file in the OS specific directory
		ConfigFilePath = CUserPaths::getAppConfigDir(DirName);

		if(!CFile::isExists(ConfigFilePath) && !CFile::createDirectoryTree(ConfigFilePath))
		{
			//or failed???
			ConfigFilePath.clear();
		}

		// If the config file doesn't exists, create it
		if(!CFile::fileExists(ConfigFilePath + ConfigFileName))
		{
			string confFP = ConfigFilePath + ConfigFileName;
/*
			if(!CFile::copyFile(confFP.c_str(), (string(TUX_TARGET_CONFIG_DIR) + ConfigFileName).c_str(), false, NULL))
			{
				nlerror ("Can't copy config template file into '%s'", confFP.c_str());
			}
*/
			//simplified code from uptream official client use config with variable RootConfigFilename

			FILE *fp = fopen(confFP.c_str(), "wt");
			if(!fp)
			{
				nlerror ("Can't create config file '%s'", confFP.c_str());
			}
			string FileContent = "RootConfigFilename = \"" + string(TUX_TARGET_CONFIG_DIR) + ConfigFileName + "\";\n";
			fprintf(fp, FileContent.c_str());
			fclose(fp);
		}
	}


	try
	{
		nlinfo("Load config file: '%s%s'", ConfigFilePath.c_str(), ConfigFileName.c_str());
		ConfigFile.load (ConfigFilePath + ConfigFileName);

		// link variables with config file
		IVariable::init(ConfigFile);
	}
	catch (EConfigFile &e)
	{
		nlinfo("Cannot launch Tux Target because there is an error in the Tux Target config file (tux-target.cfg).\nFix the error and launch Tux Target after.\n\n%s", e.what());
		exit(EXIT_FAILURE);
	}
}

void CConfigFileTask::update()
{
	CConfigFile::checkConfigFiles();
}
