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
// This class manages the loading of a level
//

#ifndef MTPT_RESOURCE_MANAGER2
#define MTPT_RESOURCE_MANAGER2


//
// Includes
//

#include <nel/misc/types_nl.h>
#include <nel/misc/mem_stream.h>
#include <nel/misc/types_nl.h>
#include <nel/misc/thread.h>
#include <nel/misc/sha1.h>

#include "resource_manager.h"



//
// Classes
//

typedef std::map<std::string, CHashKey> filename2CRC;
class CResourceManagerRunnable;

class CResourceManager : public NLMISC::CSingleton<CResourceManager>, public ITask
{
public:

	virtual void init();
	virtual void update() { };
	virtual void render() { };
	virtual void release() { };
	
	virtual std::string name() const { return "CResourceManager"; }
	
	// return empty string it a problem occurs or the full path of the file
	std::string get(const std::string &filename);
	
	// ok is true if the file is found, false is not
	std::string get(const std::string &filename, bool &ok);

	void connected(bool c);
	bool connected();
	
	std::string getFile(std::string &filename,bool now=true);

private:
	//bool CResourceManager::downloadFile (const std::string &source, const std::string &dest);
	void loadChildren(const std::string &filename);
	void update3DWhileDownloading();
		
	std::string			CacheDirectory;
	bool				Connected;
	friend int myProgressFunc(void *foo, double t, double d, double ultotal, double ulnow);	

	CResourceManagerRunnable	*ResourceManagerRunnable;
	NLMISC::IThread				*ResourceManagerThread;
	friend class CResourceManagerRunnable;
	std::string HttpServerFile;
	filename2CRC CRCs;
};

#endif
