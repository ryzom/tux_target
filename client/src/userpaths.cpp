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

#include "userpaths.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <nel/misc/debug.h>
#include <nel/misc/path.h>

// This is needed to look up the user's own home directory.
#ifdef NL_OS_WINDOWS
#include "shlobj.h"
#endif

using namespace std;

#ifdef NL_OS_UNIX

string CUserPaths::getXDGDir(const string &envName, const string &dirName)
{
	char *envXDGHome = getenv (envName.c_str());

	string xdgDir;
	if (NULL == envXDGHome || '\0' == envXDGHome[0])
	{
		xdgDir = addSlash(getHomeDir()) + dirName + "/";
		if (!isDir(xdgDir))
		{
			if (0 != mkdir (xdgDir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) )
			{
				//glib use temp here
				nlerror ("Can't create dir %s", xdgDir.c_str());
			}
		}
	}
	else
	{
		xdgDir = string(envXDGHome) + "/";
	}
	return xdgDir;
}

string CUserPaths::getConfigXDGDir()
{
	return getXDGDir("XDG_CONFIG_HOME", ".config");
}

string CUserPaths::getCacheXDGDir()
{
	return getXDGDir("XDG_CACHE_HOME", ".cache");
}

#endif

string CUserPaths::getHomeDir()
{
#ifdef NL_OS_WINDOWS
	std::string homeDir;
	// Returns something like:
	// C:\\Documents and Settings\\myuser\\Application Data
	TCHAR szAppData[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);

	homeDir.assign(szAppData);
	homeDir += "\\";
	return homeDir;
#else
	char *home = getenv("HOME");
	if (NULL == home || '\0' == home[0])
	{
		nlerror ("No home dir");
	}
	return string(home);
#endif
}

string CUserPaths::getAppConfigDir(const string &appDirName)
{
#ifdef NL_OS_WINDOWS
	std::string AppConfigDir=getHomeDir();
#else
	string AppConfigDir = getConfigXDGDir() + appDirName + "/";
	if (!isDir(AppConfigDir))
	{
		if (0 != mkdir(AppConfigDir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR))
			nlerror("Can't create dir %s", AppConfigDir.c_str());
	}

#endif
	return AppConfigDir;
}

bool CUserPaths::isDir(const string &path)
{
	//struct stat sInfo;
	//if (0 != stat (path.c_str(), &sInfo))
	//return false;

	//return bool(S_ISDIR(sInfo.st_mode));
	return NLMISC::CFile::isDirectory(path);
}

bool CUserPaths::isFile(const string &path)
{
	//struct stat sInfo;
	//if (0 != stat(path.c_str(), &sInfo))
	//return false;

	//return bool(S_ISREG(sInfo.st_mode));
	return !isDir(path);
}

string CUserPaths::addSlash(const string &path)
{
	if (path.size() > 0)
	{
	if (path[path.size() - 1] == '/')
		return path;
	else
		return path + "/";
	}

	return "/";
}
