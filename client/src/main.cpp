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

#ifdef NL_OS_WINDOWS
#	define CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
// these defines is for IsDebuggerPresent(). it'll not compile on windows 95
// just comment this and the IsDebuggerPresent to compile on windows 95
#	define _WIN32_WINDOWS	0x0410
#	define WINVER			0x0400
#	define NOMINMAX
#	include <windows.h>
HINSTANCE ghInstance = 0;
#endif

#include <string>

#include "mtp_target.h"
#include "main.h"
#include "3d_task.h"
#include "intro_task.h"
#include "task_manager.h"
#include "network_task.h"
	

//
// Namespaces
//
	
using namespace std;
using namespace NLMISC;
	

//
// Variables
//

bool DisplayDebug = false;
bool FollowEntity = false;
string ReplayFile;
sint32 AutoServerId = -1;

string crashcallback()
{
	std::string str;

	std::string OS, Proc, Mem, Gfx, Gfx2;
	OS = CSystemInfo::getOS().c_str();
	Proc = CSystemInfo::getProc().c_str();
	Mem = toString(CSystemInfo::availablePhysicalMemory()) + " | " + toString(CSystemInfo::totalPhysicalMemory());
	if(C3DTask::instance().isDriverAvailable())
	{
		Gfx = C3DTask::instance().driver().getDriverInformation();
		Gfx2 = C3DTask::instance().driver().getVideocardInformation();
	}
	else
	{
		Gfx = Gfx2 = "3d driver is not created";
	}
	str += "OS: "+OS+"\r\n";
	str += "PROC: "+Proc+"\r\n";
	str += "MEM: "+Mem+"\r\n";
	str += "GFX DRIVER: "+Gfx+"\r\n";
	str += "GFX CARD: "+Gfx2+"\r\n";

	//str += "\r\n";
	return str;
}
uint TaskManagerThreadId = 0;
uint NetworkThreadId = 0;


#ifdef NL_OS_WINDOWS

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	NLMISC::CApplicationContext myApplicationContext;
/*	int tmp;
	tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmp = (tmp & 0x0000FFFF) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(tmp);
*/
	ghInstance = hInstance;
	// Look the command line to see if we have a cookie and a addr

	nlinfo ("args: '%s'", lpCmdLine);

	string cmd = lpCmdLine;

	ReplayFile = "";
	AutoServerId = -1;
	string autoConnectFlag = "--autoconnect:";
	
	if (cmd.find ("\"") != string::npos)
	{
		// it s a replay, remove ""
		ReplayFile = cmd.substr(1, cmd.size()-2);
	}
	else if(cmd.find(autoConnectFlag) != string::npos)
	{
		size_t startIndex = cmd.find(autoConnectFlag) + autoConnectFlag.size();
		string strId = cmd.substr(startIndex,cmd.size()-startIndex);
		fromString(strId,AutoServerId);
	}
	else
		ReplayFile = cmd;

	if(!IsDebuggerPresent() && !ReplayFile.empty())
	{
		char exePath  [512] = "";
		DWORD success = GetModuleFileName (NULL, exePath, 512);
		bool found = false;
		if(success)
		{
			int l = strlen(exePath);
			for(int i=l;i>=0;i--)
			{
				if(exePath[i]=='\\')
				{
					exePath[i] = '\0';
					found = true;
					break;
				}
			}
		}
		if(found)
		{
			nlinfo("Set current dir to '%s'", exePath);
			SetCurrentDirectoryA(exePath);
		}
	}

#else

#ifdef NL_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

int main(int argc, char **argv)
{
	NLMISC::CApplicationContext myApplicationContext;
	if (argc == 2)
	{
		ReplayFile = argv[1];
	}

#ifdef NL_OS_MAC
	CFBundleRef bundle;
	bundle = CFBundleGetMainBundle();
	nldebug("main %p", bundle);
	UInt32 ver = CFBundleGetVersionNumber (bundle);
	nlinfo("version %d", ver);
	CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundle);
	if (ver && url)
	{
		CFStringRef	str;
		str = CFURLCopyFileSystemPath(CFURLCopyAbsoluteURL(url), kCFURLPOSIXPathStyle);
		nlinfo("PATH %p %p %s", url, str, CFStringGetCStringPtr(str, CFStringGetSmallestEncoding(str)));
		chdir(CFStringGetCStringPtr(str, CFStringGetSmallestEncoding(str)));
	}
#endif // NL_OS_MAC

#endif
	
	string OS, Proc, Mem, Gfx;
	OS = CSystemInfo::getOS().c_str();
	Proc = CSystemInfo::getProc().c_str();
	Mem = toString(CSystemInfo::availablePhysicalMemory()) + " | " + toString(CSystemInfo::totalPhysicalMemory());
	nlinfo("OS : %s", OS.c_str());
	nlinfo("PRC: %s", Proc.c_str());
	nlinfo("MEM: %s", Mem.c_str());

	TaskManagerThreadId = getThreadId();
	// add the main task
	CTaskManager::instance().add(CMtpTarget::instance(), 70);

	// start the execution
	CTaskManager::instance().execute();

	// return
	return EXIT_SUCCESS;
}

