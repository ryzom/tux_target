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

#include <nel/misc/md5.h>

#include <nel/net/callback_client.h>

//#include "web.h"
#include "3d_task.h"
#include "gui_task.h"
#include "time_task.h"
#include "game_task.h"
#include "gui_check.h"
#include "intro_task.h"
#include "mtp_target.h"
#include "login_client.h"
#include "font_manager.h"
#include "network_task.h"
#include "task_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "../../common/async_job.h"


//
// Namespaces
//

using namespace NL3D;
using namespace NLMISC;
using namespace NLNET;



//
// Variables
//

//guiSPG<CGuiFrame> testFrame;


uint32	ServerId = -1;
bool		TryToLog = false;
bool		AutoLogin;
bool		ReloadInterfaces = false;

//
// Classes
//

/*extern "C"
{
	void webClickCallback(OLECHAR *url)
	{
		static uint i = 0;
		if(i++==1)
		{
			ucstring u((ucchar*)url);
			openURL(u.toUtf8().c_str());
			exit(0);
		}
	}
};*/


//
// Functions
//

void CIntroTask::displayLoginFrame()
{
	CGuiObjectManager::instance().displayFrame("login");
	CGuiObjectManager::instance().addFrame("donation");
}

void CIntroTask::init()
{
	CTaskManager::instance().add(CBackgroundTask::instance(), 59);
	AutoLogin = CConfigFileTask::instance().configFile().getVar("AutoLogin").asInt() == 1;
	AutoServerId = CConfigFileTask::instance().configFile().getVar("AutoServerId").asInt();

	bool langexists = loadLang();

	loadInterfaces();

	//
	// Set the default frame
	//

	if(langexists)
	{
	 	displayLoginFrame();
	}
	else
	{
		CGuiObjectManager::instance().displayFrame("languages");
	}

	if(LatestVersion > CurrentVersion)
	{
		// need to get the latest version of the game to play
		string ext;
#ifdef NL_OS_WINDOWS
		ext = "exe";
#elif defined(NL_OS_MAC)
		ext = "dmg";
#elif defined(NL_OS_UNIX)
		ext = "tar.bz2";
#endif
		string url("http://www.mtp-target.org/files/mtp-target-setup."+toString(LatestVersion)+"."+ext);

		displayMessage(ucstring("GuiNewVersion"), new CGuiOpenUrlButtonEventBehaviour(url, true), new CGuiExitButtonEventBehaviour);
	}

	//CGuiObjectManager::instance().displayFrame("help1");

	//	if(CConfigFileTask::instance().configFile().getVar("CustomGui").asInt()>0)
	//		CGuiCustom::instance().load("data/gui/custom/");

/* web integration test
	CScissor s;
	s.init(0,0,1,0.5);
	C3DTask::instance().driver().setScissor(s);

	MSG			msg;

	// Initialize the OLE interface. We do this once-only.
	if (OleInitialize(NULL) == S_OK)
	{
		WNDCLASSW wc;
		memset (&wc,0,sizeof(wc));
		wc.style			= CS_HREDRAW | CS_VREDRAW ;//| CS_DBLCLKS;
		wc.lpfnWndProc		= (WNDPROC)WebWindowProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= GetModuleHandleW(NULL);
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursorW(NULL,(LPWSTR)IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)COLOR_WINDOW;
		wc.lpszClassName	= L"toto";
		wc.lpszMenuName		= NULL;
		if ( !RegisterClassW(&wc) ) return;

		ULONG	WndFlags;
		RECT	WndRect;
		WndFlags = WS_POPUPWINDOW;// | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		WndRect.left = 0;
		WndRect.top = 0;
		WndRect.right = 400;
		WndRect.bottom = 200;
		AdjustWindowRect(&WndRect,WndFlags,FALSE);

		// Create another window with another browser object embedded in it.
		//msg.hwnd = CreateWindowEx(0, LPCWSTR(""), LPCWSTR("Microsoft's web site"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_DESKTOP, NULL, NULL, 0);
		//msg.hwnd = CreateWindowW (L"toto", L"test", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, hWnd, NULL, GetModuleHandle(NULL), NULL);
		msg.hwnd = CreateWindowW (L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_SIMPLE, 0, 0, 728, 90, (HWND)C3DTask::instance().driver().getDisplay(), NULL, (HINSTANCE) GetWindowLong((HWND)C3DTask::instance().driver().getDisplay(), GWL_HINSTANCE), NULL);
		if (msg.hwnd)
		{
			if (EmbedBrowserObject(msg.hwnd)) nlstop;

			// For this window, display a URL. This could also be a HTML file on disk such as "c:\\myfile.htm".
			DisplayHTMLPage(msg.hwnd, L"http://www.mtp-target.org/t/ad.php");

			// Show the window.
			ShowWindow(msg.hwnd, 1);
			UpdateWindow(msg.hwnd);
		}
	}
*/
}

bool CIntroTask::loadLang()
{
	// load the translation file
	bool exists = false;
	string lang("en");
	string cf(CConfigFileTask::instance().tempDirectory()+"lang.txt");
	if(!cf.empty())
	{
		FILE *fp = fopen(cf.c_str(), "rb");
		if(fp)
		{
			char line[20];
			fgets(line, 20, fp);
			uint l = strlen(line);
			if(l > 0)
			{
				if(line[l-1] == '\n') line[l-1] = '\0';
				lang = line;
				exists = true;
			}
			fclose(fp);
		}
	}
	CI18N::load(lang, "en");
	return exists;
}

void CIntroTask::render()
{
	CFontManager::instance().print(CFontManager::TCDebug, 0, 0, "v" + ucstring(ReleaseVersion));
}

void CIntroTask::update() 
{
	if(ReloadInterfaces)
	{
		loadLang();
		loadInterfaces();
		displayLoginFrame();
		ReloadInterfaces = false;
	}

	if(TryToLog && ServerId != -1)
	{
		CGuiObjectManager::instance().clear();

		CInetAddress ip;
		string cookie;
		//nlinfo("CLoginClientMtp::connectToShard(%d)",ServerId);
		ucstring reason = CLoginClientMtp::connectToShard(ServerId,ip,cookie);
		if(!reason.empty())
		{
			AutoLogin = false;
			TryToLog = false;
			CIntroTask::instance().displayLoginFrame();
			error(reason);
			return;
		}

		//nlinfo("CNetworkTask::instance().connect()");
		reason = CNetworkTask::instance().connect(ip, cookie);
		if(!reason.empty())
		{
			AutoLogin = false;
			TryToLog = false;
			CIntroTask::instance().displayLoginFrame();
			error(reason);
			return;
		}

		CGuiObjectManager::instance().clear();
		AutoLogin = false; // autologin only once

		// stop the interface task
		CIntroTask::instance().stop();

		// stop the background task
		CBackgroundTask::instance().stop();

		// go to the game task
		CTaskManager::instance().add(CGameTask::instance(), 5);

		TryToLog = false;
	}
}

void CIntroTask::loadInterfaces() 
{
	loadFrameDonation();
	loadFrameError();
	loadFrameHelp();
	loadFrameLanguages();
	loadFrameLogin();
	loadFrameMessage();
	loadFrameRegister();
	loadFrameSettings();
}

void CIntroTask::doConnection( uint32 serverId ) 
{
	ServerId = serverId;
	TryToLog = true;
}