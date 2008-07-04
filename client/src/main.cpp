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

#include <nel/misc/report.h>
#include <nel/misc/command.h>

#include <nel/net/tcp_sock.h>

#include "main.h"
#include "3d_task.h"
#include "intro_task.h"
#include "mtp_target.h"
#include "task_manager.h"
#include "network_task.h"
#include "config_file_task.h"


//
// Namespaces
//
	
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//

uint8 DisplayDebug = 0;
bool FollowEntity = true;
string ReplayFile;
sint32 AutoServerId = -1;


//
// Functions
//

string crashcallback()
{
	string str;
	str  = "ServerId="+toString(CIntroTask::instance().serverId())+"\r\n";
	str += "Login="+login().toUtf8()+"\r\n";

	string OS, Proc, Mem, Gfx, Gfx2;
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

string escapeHTML(const string &str)
{
	string res;
	for(uint i = 0; i < str.size(); i++)
	{
		if(isalnum(str[i])) res += str[i];
		else res += toString("%%%02x",str[i]);
	}
	return res;
}

bool sendhtmlreport(const string &smtpServer, const string &from, const string &to, const string &subject, const string &body, const string &attachedFile, bool onlyCheck)
{
	string CrashReportServer("www.mtp-target.org");
	CTcpSock sock;
	sock.connect(CInetAddress(CrashReportServer, 80));

	breakable
	{
		if (!sock.connected())
		{
			nlwarning ("Can't connect to http server %s", CrashReportServer.c_str());
			break;
		}

		string Content = "subject="+escapeHTML(subject)+"&body="+escapeHTML(body);

		string buffer;
		buffer += "POST /mt/mt_crash_report.php HTTP/1.0\r\n";
		buffer += "User-Agent: NeLCrashReport/1.0\r\n";
		buffer += "Content-Type: application/x-www-form-urlencoded\r\n";
		buffer += "Content-Length: "+toString(Content.size())+"\r\n\r\n";
		buffer += Content;

		uint32 size = buffer.size();
		if (sock.send ((uint8 *)buffer.c_str(), size) != CSock::Ok)
		{
			nlwarning ("Can't send data to the server");
			break;
		}
		string res;
		char c;
		while (true)
		{
			size = 1;
			
			if (sock.receive((uint8*)&c, size, false) == CSock::Ok)
			{
				if(c == '\n')
				{
					nlinfo(res.c_str());
					res.clear();
				}
				else
					res += c;
			}
			else
			{
				nlinfo("Connection closed");
				break;
			}
		}
	}
	return true;
}

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

	CConfigFileTask::instance().setTempDirectory();
	if(NLMISC::CFile::fileExists(CConfigFileTask::instance().tempDirectory()+"log.log")) NLMISC::CFile::deleteFile(CConfigFileTask::instance().tempDirectory()+"log.log");
	createDebug(CConfigFileTask::instance().tempDirectory().c_str(), true, true);
	nlinfo("Log in : '%slog.log'", CConfigFileTask::instance().tempDirectory().c_str());

	// Look the command line to see if we have a cookie and a addr
	nlinfo ("args: '%s'", lpCmdLine);
	string cmd = lpCmdLine;
	ReplayFile.clear();
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
	else if(!cmd.empty())
	{
		if(CFile::getExtension(cmd) == "mtr" && CFile::fileExists(cmd))
			ReplayFile = cmd;
		else
			nlwarning("File passed in the command line doesn't exists or is not a .mtr '%s'", cmd.c_str());
	}

	if(!CFile::fileExists("mtp_target_default.cfg") && !IsDebuggerPresent() && !ReplayFile.empty())
	{
		char exePath  [512] = "";
		DWORD success = GetModuleFileNameA (NULL, exePath, 512);
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

	CConfigFileTask::instance().setTempDirectory();
	if(NLMISC::CFile::fileExists(CConfigFileTask::instance().tempDirectory()+"log.log")) NLMISC::CFile::deleteFile(CConfigFileTask::instance().tempDirectory()+"log.log");
	createDebug(CConfigFileTask::instance().tempDirectory().c_str(), true, true);
	nlinfo ("Log in : '%slog.log'", CConfigFileTask::instance().tempDirectory().c_str());

	if (argc == 2)
	{
		ReplayFile = argv[1];
		if(CFile::getExtension(ReplayFile) != "mtr")
			ReplayFile.clear();
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

	setCrashCallback(crashcallback);
	setReportEmailFunction((void*)sendhtmlreport);

	// add the main task
	CTaskManager::instance().add(CMtpTarget::instance(), 70);

	// start the execution
	CTaskManager::instance().execute();

	// return
	return EXIT_SUCCESS;
}
