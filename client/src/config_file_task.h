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

#ifndef MT_CONFIG_FILE_TASK_H
#define MT_CONFIG_FILE_TASK_H


//
// Includes
//

#include <nel/misc/singleton.h>


//
// Classes
//

class CConfigFileTask : public NLMISC::CSingleton<CConfigFileTask>, public ITask
{
public:
	
	virtual void init();
	virtual void update();
	virtual void render() { }
	virtual void release() { }

	virtual string name() const { return "CConfigFileTask"; }

	NLMISC::CConfigFile &configFile() { nlassert(ConfigFile.loaded()); return ConfigFile; }

	const string &cacheDirectory() const { return CacheDirectory; }
	const string &tempDirectory() const { return TempDirectory; }
	const string &documentDirectory() const { return DocumentDirectory; }
	void setTempDirectory();

private:

	void setDocumentDirectory();

	NLMISC::CConfigFile ConfigFile;	// In the current directory or "C:/Documents and Settings/user/Application Data/Mtp Target" or "~/.mtp_target/"

	string CacheDirectory;	// tmpdir + "cache/"
	string TempDirectory;	// "C:/Documents and Settings/user/Local Settings/Temp/Mtp Target/" or "/tmp/mtp_target/"
	string DocumentDirectory;	// "C:/Documents and Settings/user/My documents/Mtp Target/" or "~/.mtp_target/"
};

#endif
