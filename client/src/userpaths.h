/* Copyright, 2010 Tux Target
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

#ifndef USER_PATHS_H
#define USER_PATHS_H

#include <nel/misc/types_nl.h>
#include <string>

class CUserPaths
{

#ifdef NL_OS_UNIX

private:
	static std::string getXDGDir(const std::string &envName, const std::string &dirName);
	static std::string getConfigXDGDir();
	static std::string getCacheXDGDir();

#endif

public:
	static std::string getHomeDir();
	static std::string getAppConfigDir(const std::string &appDirName);
	static bool isDir(const std::string &path);
	static bool isFile(const std::string &path);
	static std::string addSlash(const std::string &path);
};

#endif
