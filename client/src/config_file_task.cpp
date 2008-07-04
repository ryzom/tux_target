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

#include <stdio.h>
#include <errno.h>

#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <windows.h>
#	include <shlobj.h>
#else
#	include <unistd.h>
#include <paths.h>
#	include <pwd.h>
#endif

#include <nel/misc/variable.h>

#include "../../common/async_job.h"
#include "config_file_task.h"
#include "mtp_target.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//

void CConfigFileTask::init()
{
	const string ConfigFileName = "mtp_target.cfg";
	string ConfigFilePath;

	// First, we look at the config file file in the current directory
	if(CFile::fileExists(ConfigFilePath+ConfigFileName))
		goto loadconfigfile;

	// Second, we look at the config file file in the OS specific directory
#ifdef NL_OS_WINDOWS
	CHAR szPath[MAX_PATH];
	if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) 
	{
		ConfigFilePath = CPath::standardizePath(string(szPath), true)+"Mtp Target/";
	}
#else
	{
		passwd *pwd = getpwuid(getuid());
		if(pwd && pwd->pw_dir)
		{
			ConfigFilePath = CPath::standardizePath(string(pwd->pw_dir), true)+".mtp_target/";
		}
	}
#endif

	if(!CFile::isExists(ConfigFilePath) && !CFile::createDirectoryTree(ConfigFilePath))
	{
		ConfigFilePath.clear();
	}

	// If the config file doesn't exists, create it
	if(!CFile::fileExists(ConfigFilePath+ConfigFileName))
	{
		FILE *fp = fopen((ConfigFilePath+ConfigFileName).c_str(), "wt");
		if(!fp)
		{
			FILE *fp = fopen(ConfigFileName.c_str(), "wt");
			nlassert(fp);
			ConfigFilePath.clear();
		}
		fprintf(fp,"RootConfigFilename = \"mtp_target_default.cfg\";\n");
		fclose(fp);
	}

loadconfigfile:
	try
	{
		nlinfo("Load config file: '%s%s'", ConfigFilePath.c_str(), ConfigFileName.c_str());
		ConfigFile.load (ConfigFilePath+ConfigFileName);

		// link variables with config file
		IVariable::init(ConfigFile);
	}
	catch (EConfigFile &e)
	{
#ifdef NL_OS_WINDOWS
		MessageBoxA(NULL, toString("Cannot launch Mtp Target because there is an error in the Mtp Target config file (mtp_target.cfg or mtp_target_default.cfg).\r\nFix the error and launch Mtp Target after.\r\n\r\n%s", e.what()).c_str(), "Error in the Mtp Target config file", MB_OK);
#else
		nlinfo("Cannot launch Mtp Target because there is an error in the Mtp Target config file (mtp_target.cfg or mtp_target_default.cfg).\nFix the error and launch Mtp Target after.\n\n%s", e.what());
#endif
		exit(EXIT_FAILURE);
	}

	CPath::remapExtension("dds", "tga", true);
	CPath::remapExtension("dds", "png", true);

	if(CConfigFileTask::instance().configFile().exists("NegFiltersDebug"))
	{
		CConfigFile::CVar &var = CConfigFileTask::instance().configFile().getVar("NegFiltersDebug");
		for(uint i = 0; i < var.size(); i++)
			DebugLog->addNegativeFilter(var.asString(i).c_str());
	}

	if(CConfigFileTask::instance().configFile().exists("NegFiltersInfo"))
	{
		CConfigFile::CVar &var2 = CConfigFileTask::instance().configFile().getVar("NegFiltersInfo");
		for(uint i = 0; i < var2.size(); i++)
			InfoLog->addNegativeFilter(var2.asString(i).c_str());
	}

	CConfigFile::CVar &v = CConfigFileTask::instance().configFile().getVar("Path");
	for (uint i = 0; i < v.size(); i++)
		CPath::addSearchPath (v.asString(i), true, false);

	// setup cache directory
	CacheDirectory = tempDirectory()+"cache/";
	if(!CFile::isExists(CacheDirectory) && !CFile::createDirectoryTree(CacheDirectory)) CacheDirectory.clear();

	setDocumentDirectory();

	if(ReplayFile.empty() && (!CConfigFileTask::instance().configFile().exists("NoVersionCheck") || CConfigFileTask::instance().configFile().getVar("NoVersionCheck").asInt() != 1))
	{
		// check if we need a new version of the game (update)
		CurrentVersion = 0;

		// get the local version of the game
		FILE *fp;
		string cf = CPath::lookup("current_version");
		fp = fopen(cf.c_str(), "rb");
		if(fp)
		{
			char cverstr[64];
			size_t size = fread(cverstr, 1, 63, fp);
			cverstr[size] = '\0';
			fclose(fp);
			CurrentVersion = uint16(atoi(cverstr));
		}
		else
		{
			nlinfo("current_version path is : '%s'", cf.c_str());
			nlerror("Cannot fopen the current_version %d", errno);
		}

		ReleaseVersion += toString(CurrentVersion);
		nlinfo("ReleaseVersion %s", ReleaseVersion.c_str());

		// get the latest version on the net
		//downloadFile("http://www.mtp-target.org/files/latest_version", "latest_version");
		// this php file returns the latest_version and information on the fundraising
		string latest_version = CConfigFileTask::instance().tempDirectory()+"latest_version";
		downloadFile("http://www.mtp-target.org/mt/mt_game_header.php", latest_version.c_str());
		//downloadFile("http://localhost/www/mt/mt_game_header.php", latest_version.c_str());
		fp = fopen(latest_version.c_str(), "rb");
		if(fp)
		{
			char lverstr[512];
			size_t size = fread(lverstr, 1, 511, fp);
			lverstr[size] = '\0';
			fclose(fp);
			CFile::deleteFile(latest_version);

			vector<string> res;
			explode(string(lverstr), string("|"), res);
			if(res.size()>=1) LatestVersion = uint16(atoi(res[0].c_str()));
			if(res.size()>=2) FundRaised = atoi(res[1].c_str());
			if(res.size()>=3) FundGoal = ucstring::makeFromUtf8(res[2]);
		}
		else
		{
			nlerror("Cannot fopen the latest_version '%s' %d", latest_version.c_str(), errno);
		}

		if(CurrentVersion == 0) nlerror("Cannot retrieve the current version");
		else if(LatestVersion == 0) nlerror("Cannot retrieve the latest version");
		/*else if(lver > cver)
		{
#ifdef NL_OS_WINDOWS
			sint val = MessageBoxA(NULL, "A new version of Mtp Target is available, you must install it to be able to play.\r\n\r\nPress OK will quit the game and automatically open your browser to download the latest version of the game.\r\nPress Cancel will just quit the game.\r\n", toString("Mtp Target Update (lver:%d cver:%d)", lver, cver).c_str(), MB_OKCANCEL);
			if(val == IDOK)
			{
				openURL("http://www.mtp-target.org/files/mtp-target-setup.exe");
			}
#else
			InfoLog->displayRawNL("");
			InfoLog->displayRawNL("A new version of Mtp Target is available, you must install it to be able to play.");
			InfoLog->displayRawNL("You can download the latest version here:");
			InfoLog->displayRawNL("http://www.mtp-target.org/files/mtp-target-setup.run");
			openURL("http://www.mtp-target.org/files/mtp-target-setup.run");
#endif
			exit(EXIT_SUCCESS);
		}*/
		else if(LatestVersion < CurrentVersion)
		{
			nlwarning("Your local version is too recent! (lver:%d cver:%d)", LatestVersion, CurrentVersion);
		}
	}
}

void CConfigFileTask::update()
{
	CConfigFile::checkConfigFiles();
}

void CConfigFileTask::setDocumentDirectory()
{
#ifdef NL_OS_WINDOWS
	CHAR szPath[MAX_PATH];
	//if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, szPath))) 
	if(SHGetSpecialFolderPathA(NULL, szPath, CSIDL_PERSONAL, false))
	{
		DocumentDirectory = CPath::standardizePath(string(szPath), true)+"Mtp Target/";
	}
#else
	passwd *pwd = getpwuid(getuid());
	if(pwd && pwd->pw_dir)
	{
		DocumentDirectory = CPath::standardizePath(string(pwd->pw_dir), true)+".mtp_target/";
	}
#endif

	if(!CFile::isExists(DocumentDirectory) && !CFile::createDirectoryTree(DocumentDirectory)) DocumentDirectory.clear();
	nlinfo("Set document directory to '%s'", DocumentDirectory.c_str());
}

void CConfigFileTask::setTempDirectory() 
{
#ifdef NL_OS_WINDOWS
	DWORD dwBufSize=512;
	CHAR lpPathBuffer[512];
	DWORD  dwRetVal = GetTempPathA(dwBufSize, lpPathBuffer);
	if (dwRetVal > dwBufSize || (dwRetVal == 0))
	{
		nlwarning("GetTempPath failed (%d)", GetLastError());
	}
	else
	{
		TempDirectory = CPath::standardizePath(string(lpPathBuffer), true)+"Mtp Target/";
	}
#else
	TempDirectory = CPath::standardizePath(string(_PATH_TMP), true)+"mtp_target/";
#endif

	if(!CFile::isExists(TempDirectory) && !CFile::createDirectoryTree(TempDirectory)) TempDirectory.clear();
	//nlinfo("Set temp directory to '%s'", TempDirectory.c_str());
}
