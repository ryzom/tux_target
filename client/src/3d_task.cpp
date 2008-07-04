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
#	define NOMINMAX
#	include <windows.h>
#	include "resource.h"
	extern HINSTANCE ghInstance;
#endif

#include <nel/misc/variable.h>

#include <nel/3d/driver.h>
//#include <3d/bloom_effect.h>

#include <nel/3d/u_visual_collision_manager.h>

#include "gui.h"
#include "level.h"
#include "fi_ime.h"
#include "3d_task.h"
#include "hud_task.h"
#include "gui_task.h"
#include "sky_task.h"
#include "end_task.h"
#include "chat_task.h"
#include "time_task.h"
#include "game_task.h"
#include "water_task.h"
#include "mtp_target.h"
#include "score_task.h"
#include "editor_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "swap_3d_task.h"
#include "task_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "lens_flare_task.h"
#include "background_task.h"
#include "config_file_task.h"
#include "external_camera_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

CFullIME fi;

CVariable<bool> Shadow("mtp", "Shadow", "Display or not dynamic shadows", true, 0, true);
CVariable<uint8> ScreenShotQuality("mtp", "ScreenShotQuality", "Quality of the jpeg", true, 80, true);

//static CBloomEffect be;

//bool FastExit = true;


//
// Functions
//

void exitFunction()
{
	//exit(0);
	CTaskManager::instance().exit();

/*	if(FullVersion || FastExit)
	{
		exit(0);
	}
	else
	{
		CGameTask::instance().remove();
		CChatTask::instance().remove();
		CSkyTask::instance().remove();
		CHudTask::instance().remove();
		CWaterTask::instance().remove();
		CMtpTarget::instance().remove();
		CLensFlareTask::instance().remove();
		CEntityManager::instance().remove();
		CExternalCameraTask::instance().remove();
		CLevelManager::instance().remove();
		CScoreTask::instance().remove();
		CGuiTask::instance().remove();
		CNetworkTask::instance().remove();

		CTaskManager::instance().add(CEndTask::instance(), 60);
	}*/
}

class CInterfaceListener : public IEventListener
{
	virtual void	operator() (const CEvent& ev)
	{
		if(ev == EventIME)
		{
			const CEventIME &e = (const CEventIME &)ev;
			//nlinfo("we have an ime event!!! %d %d %d", e.EventMessage, e.WParam, e.LParam);
			bool trap = false;
			if (fi.msgProc( e.EventMessage, e.WParam, e.LParam ) )
				trap = true;
			if ( fi.staticMsgProc( e.EventMessage, e.WParam, e.LParam ) )
				trap = true;

#ifdef NL_OS_WINDOWS
			// The message was trapped by the event emitter, untrap it since msgProc or staticMsgProc says so
			if(!trap)
				::DefWindowProc( (HWND)C3DTask::instance().driver().getDisplay(), e.EventMessage, e.WParam, e.WParam );
#endif
		}
		else
		{
			if ( ! _MaxWidthReached )
			{
				CEventChar &ec = (CEventChar&)ev;
				ucchar c = ec.Char;
				if(c != 9)	// discard TAB
				{
					//nldebug("---------- Adding char %u '%c'", c, c);
					_Line += c;
				}
			}
		}
	}

public:
	CInterfaceListener() : _MaxWidthReached( false )
	{}

	virtual ~CInterfaceListener() {}

	const ucstring&	line() const
	{
		return _Line;
	}

	void setLine(const ucstring &str) { _Line = str; }
	void addLine(const ucstring &str)
	{
		//for (uint i = 0; i < str.size(); i++)
			//nldebug("---------- %u Adding char %u '%c'", i, str[i], str[i]);
		_Line += str;
	}

	void			setMaxWidthReached( bool b )
	{
		_MaxWidthReached = b;
	}

private:
	ucstring		_Line;
	bool			_MaxWidthReached;
	ucstring		_LastCommand;
};

static CInterfaceListener InterfaceListener;

void C3DTask::init()
{
	ScreenWidth = CConfigFileTask::instance().configFile().getVar("ScreenWidth").asInt();
	ScreenHeight = CConfigFileTask::instance().configFile().getVar("ScreenHeight").asInt();

	CConfigFile::CVar v;

	v = CConfigFileTask::instance().configFile().getVar("ClearColor");
	nlassert(v.size()==4);
	ClearColor.set(v.asInt(0),v.asInt(1),v.asInt(2),v.asInt(3));

	// Create a driver
	uint icon = 0;
#ifdef NL_OS_WINDOWS
	icon = (uint)LoadIcon(ghInstance,MAKEINTRESOURCE(IDI_ICON1));
#endif
	Driver = UDriver::createDriver(icon, true, exitFunction);
	nlassert(Driver);

	if(CConfigFileTask::instance().configFile().getVar("VSync").asInt() == 0)
		Driver->setSwapVBLInterval(0);

	if(CConfigFileTask::instance().configFile().getVar("NativeFragmentProgramsOnly").asInt() == 0)
		Driver->forceNativeFragmentPrograms(false);

#ifdef NL_OS_WINDOWS
	Fullscreen = CConfigFileTask::instance().configFile().getVar("Fullscreen").asInt()==1;
#else
	// no fullscreen on linux
	Fullscreen = false;
#endif

	uint8 aa = (CConfigFileTask::instance().configFile().getVar("AntiAlias").asInt()==1)?2:-1;

	bool displayOk = false;
	string reason;
	try
	{
		displayOk = Driver->setDisplay (UDriver::CMode(ScreenWidth, ScreenHeight, CConfigFileTask::instance().configFile().getVar("ScreenDepth").asInt(), !Fullscreen, CConfigFileTask::instance().configFile().getVar("ScreenFrequency").asInt(), aa), true, false);
	}
	catch (EBadDisplay &e)
	{
		displayOk = false;
		reason = e.what();
	}

	// Create the window with config file values
	if (!displayOk)
	{
		nlwarning ("Can't set display mode %dx%d %dbpp %dHz %d", ScreenWidth, ScreenHeight, CConfigFileTask::instance().configFile().getVar("ScreenDepth").asInt(), CConfigFileTask::instance().configFile().getVar("ScreenFrequency").asInt(), !Fullscreen);
		if(!reason.empty()) nlwarning ("Reason: %s", reason.c_str());

		string OS, Proc, Mem, Gfx, Gfx2;
		OS = CSystemInfo::getOS().c_str();
		Proc = CSystemInfo::getProc().c_str();
		Mem = toString(CSystemInfo::availablePhysicalMemory()) + " | " + toString(CSystemInfo::totalPhysicalMemory());
		Gfx = Driver->getDriverInformation();
		Gfx2 = Driver->getVideocardInformation();
		nlinfo("OS: %s", OS.c_str());
		nlinfo("PROC: %s", Proc.c_str());
		nlinfo("MEM: %s", Mem.c_str());
		nlinfo("GFX DRIVER: %s", Gfx.c_str());
		nlinfo("GFX CARD: %s", Gfx2.c_str());

		vector<UDriver::CMode> modes;
		bool res = Driver->getModes(modes);
		vector<UDriver::CMode>::iterator it;
		nlinfo("Available video mode:");
		if(modes.size()==0) nlinfo("   no video mode...");

		for(it=modes.begin();it!=modes.end();it++)
		{
			UDriver::CMode m = *it;
			nlinfo("   > %dx%d %dbpp %dHz %s",m.Width,m.Height,m.Depth,m.Frequency,m.Windowed?"windowed":"fullscreen");
		}

#ifdef NL_OS_WINDOWS
		MessageBoxA (NULL, "Your graphic card is not supported by Mtp Target.\r\n\r\nTry to update your driver.\r\nIf you have modified mtp_target.cfg, be sure the resolution is ok. Or post a message on the forum.", "Error", MB_OK);
#endif
		exit(EXIT_FAILURE);
	}

	fi.init();

	Driver->EventServer.addListener (EventCharId, &InterfaceListener);
	Driver->EventServer.addListener (EventIME, &InterfaceListener);
	//Driver->enableLowLevelKeyboard(true);

	// create a scene
	Scene = Driver->createScene(false);

	if (Scene == 0)
	{
		nlwarning ("Can't create nel scene");
		return;
	}

	//Scene->getCam()->setFrustum(0.26f, 0.2f, 0.1f, 40.0f);
	Scene->getCam().setPerspective(degToRad(CConfigFileTask::instance().configFile().getVar("Fov").asFloat()), 1.33f, 1.0f*GScale, 30000.0f*GScale);
	Scene->getCam().setTransformMode(UTransformable::DirectMatrix);

	MouseListener = new C3dMouseListener();
	MouseListener->init();
	MouseListener->addToServer(driver().EventServer);
	//CaptureCursor = false;

	Scene->enableLightingSystem(true);

	Scene->setPolygonBalancingMode(UScene::PolygonBalancingOn);
	Scene->setGroupLoadMaxPolygon("Fx", CConfigFileTask::instance().configFile().getVar("FxNbMaxPoly").asInt());

//	be.init(false, Driver, Scene);

	if(Shadow)
	{
		CollisionManager = Scene->createVisualCollisionManager();
		nlassert(CollisionManager);
		// Set this collision manager the one the scene must use for Shadow Reception on Buildings
		Scene->setVisualCollisionManagerForShadow(CollisionManager);
		CollisionManager->setPlayerInside(true);
	}

	fi.onFocus(true);

	Driver->setWindowTitle("Mtp Target "+ReleaseVersion);
}

void C3DTask::update()
{
	if(!Driver->isActive())
		exitFunction();
	if(kbPressed(KeyESCAPE))
		exitFunction();

	Scene->animate(CTimeTask::instance().time());
	Driver->EventServer.pump(true);
	if(kbDown(KeySHIFT) && kbPressed(KeyF1))
		CTaskManager::instance().switchBench();
}

void C3DTask::takeScreenShot()
{
	if(!CConfigFileTask::instance().configFile().exists("ScreenShot") || CConfigFileTask::instance().configFile().getVar("ScreenShot").asInt() == 0)
		return;

	string path = CConfigFileTask::instance().documentDirectory();
	if(!CFile::isExists(path) && !CFile::createDirectoryTree(path)) path.clear();

	CBitmap btm;
	C3DTask::instance().driver().getBuffer (btm);
	string filename = CFile::findNewFile (path+"screenshot.jpg");
	COFile fs(filename);
	btm.writeJPG(fs, ScreenShotQuality);
	nlinfo("Screenshot '%s' saved", filename.c_str());
}

void C3DTask::render()
{
	Driver->enableFog(true);

//	be.initBloom();

	try
	{
		Scene->render();
	}
	catch (EFile &e)
	{
		// This can happen when a texture was badly downloaded
		if(e.Filename.find("/cache/") != string::npos)
		{
			nlwarning("The file '%s' is corrupted, I delete it", e.Filename.c_str());
			CFile::deleteFile(e.Filename);
		}
		else
		{
			nlerror("%s / %s", e.what(), e.Filename.c_str());
		}
	}

	C3DTask::instance().driver().enableFog(false);

	if(CEntityManager::instance().execute())
		CEntityManager::instance().renderNames();

//	be.endBloom();
//	be.endInterfacesDisplayBloom();
}

void C3DTask::release()
{
	fi.release();
	nlassert(Driver);
	Driver->release();
}

bool C3DTask::kbPressed(NLMISC::TKey key) const
{
	return Driver->AsyncListener.isKeyPushed(key);
}

bool C3DTask::kbDown(NLMISC::TKey key) const
{
	return Driver->AsyncListener.isKeyDown(key);
}

ucstring C3DTask::kbGetString() const
{
	ucstring str = InterfaceListener.line();
	InterfaceListener.setLine(ucstring(""));
	return str;
}

void C3DTask::kbAddToString(const ucstring &str)
{
	nldebug("adding to the current string '%s'", str.toUtf8().c_str());
	InterfaceListener.addLine(str);
}

void C3DTask::clear()
{
	Driver->clearBuffers (ClearColor);
}

void C3DTask::captureCursor(bool b)
{
	CGuiObjectManager::instance().mouseListener().captureCursor(b);
	/*
	CaptureCursor = b;
	if(b)
	{
		driver().setCapture(true);
		driver().showCursor(false);
		mouseListener().addToServer(driver().EventServer);
	}
	else
	{
		driver().setCapture(false);
		driver().showCursor(true);
		mouseListener().removeFromServer(driver().EventServer);
	}
	*/
}

NL3D::UMaterial C3DTask::createMaterial() const
{
	nlassert(Driver);
	UMaterial m = driver().createMaterial();
	m.initUnlit();
	return m;
}
