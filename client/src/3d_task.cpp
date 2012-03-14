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
#	define NOMINMAX
#	include <windows.h>
#	include "../resource.h"
	extern HINSTANCE ghInstance;
#endif

#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/material.h>
#include <nel/3d/register_3d.h>

#include "gui.h"
#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "editor_task.h"
#include "task_manager.h"
#include "entity_manager.h"
#include "config_file_task.h"
#include "level_manager.h"
#include "resource_manager2.h"
	

//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//

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
	virtual void	operator() ( const CEvent& event )
	{
		if ( ! _MaxWidthReached )
		{
			char c = (char)((CEventChar&)event).Char;
			if(CEditorTask::getInstance().enable())
			{
				if(c=='4' && C3DTask::getInstance().kbDown(KeyNUMPAD4))
					return;
				if(c=='6' && C3DTask::getInstance().kbDown(KeyNUMPAD6))
					return;
				if(c=='8' && C3DTask::getInstance().kbDown(KeyNUMPAD8))
					return;
				if(c=='5' && C3DTask::getInstance().kbDown(KeyNUMPAD5))
					return;
			}
			_Line += c;
		}
	}
	
public:
	CInterfaceListener() : _MaxWidthReached( false )
	{}
	
	virtual ~CInterfaceListener() {}
	
	const string&	line() const
	{
		return _Line;
	}
	
	void setLine(const string &str) { _Line = str; }
	
	void			setMaxWidthReached( bool b )
	{
		_MaxWidthReached = b;
	}
	
private:
	string			_Line;
	bool			_MaxWidthReached;
	string			_LastCommand;
};

static CInterfaceListener InterfaceListener;

void C3DTask::init()
{
	ScreenWidth = CConfigFileTask::instance().configFile().getVar("ScreenWidth").asInt();
	ScreenHeight = CConfigFileTask::instance().configFile().getVar("ScreenHeight").asInt();

	EnableExternalCamera = false;

	CConfigFile::CVar v;
	v = CConfigFileTask::getInstance().configFile().getVar("AmbientColor");
	nlassert(v.size()==4);
	AmbientColor.set(v.asInt(0),v.asInt(1),v.asInt(2),v.asInt(3));
	v = CConfigFileTask::getInstance().configFile().getVar("ClearColor");
	nlassert(v.size()==4);
	ClearColor.set(v.asInt(0),v.asInt(1),v.asInt(2),v.asInt(3));


	// Create a driver
	uint icon = 0;
#ifdef NL_OS_WINDOWS
	icon = (uint)LoadIcon(ghInstance,MAKEINTRESOURCE(IDI_ICON1));
#endif
	bool useD3D = !CConfigFileTask::instance().configFile().getVar("OpenGL").asBool();
	Driver = UDriver::createDriver(icon, useD3D, exitFunction);
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
	std::string reason;
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

		std::string OS, Proc, Mem, Gfx, Gfx2;
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
		std::vector<UDriver::CMode>::iterator it;
		nlinfo("Available video mode: ");
		if(modes.size()==0) nlinfo("   no video mode...");

		for(it=modes.begin();it!=modes.end();it++)
		{
			UDriver::CMode m = *it;
			nlinfo("   > %dx%d %dbpp %dHz %s",m.Width,m.Height,m.Depth,m.Frequency,m.Windowed?"windowed":"fullscreen");
		}

#ifdef NL_OS_WINDOWS
		MessageBoxA (NULL, "Your graphic card is not supported by Tux Target.\r\n\r\nTry to update your driver.\r\nIf you have modified tux_target.cfg, be sure the resolution is ok. Or post a message on the forum.", "Error", MB_OK);
#endif
		exit(EXIT_FAILURE);
	}

	Driver->EventServer.addListener (EventCharId, &InterfaceListener);
	//Driver->enableLowLevelKeyboard(true);

	Driver->setAmbientColor(AmbientColor);

	Driver->enableFog(false);
	Driver->setupFog(CConfigFileTask::getInstance().configFile().getVar("FogDistMin").asFloat(),CConfigFileTask::getInstance().configFile().getVar("FogDistMax").asFloat(),ClearColor);

	// Create a scene
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
	Scene->setSunAmbient(AmbientColor);
	Scene->setSunDiffuse(CRGBA(255,255,255));
	Scene->setSunSpecular(CRGBA(255,255,255));
	Scene->setSunDirection(CVector(-1,0,-1));

	Scene->setPolygonBalancingMode(UScene::PolygonBalancingOn);
	Scene->setGroupLoadMaxPolygon("Fx", CConfigFileTask::getInstance().configFile().getVar("FxNbMaxPoly").asInt());


	LevelParticle = 0;
	//too much particles , no left to trace.ps
	if(CConfigFileTask::getInstance().configFile().getVar("DisplayParticle").asInt() == 1)
	{
		string res;
		res = CResourceManager::getInstance().get("snow.ps");
		LevelParticle.cast(C3DTask::getInstance().scene().createInstance(res));
		if (!LevelParticle.empty())
		{
			LevelParticle.setTransformMode(UTransformable::RotQuat);
			LevelParticle.hide();
			LevelParticle.setOrderingLayer(2);
		}
	}

	// TODO MTR find ReleaseVersion
//	Driver->setWindowTitle("Mtp Target "+ReleaseVersion);
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
	CBitmap btm;
	C3DTask::getInstance().driver().getBuffer (btm);
	//		btm.flipV();
	string filename = "";
	if(CMtpTarget::getInstance().sessionFileName().size())
		filename = CFile::findNewFile(CFile::getFilenameWithoutExtension(CMtpTarget::getInstance().sessionFileName())+"_.jpg");
	else
		filename = CFile::findNewFile ("screenshot.jpg");
	COFile fs (filename);
	btm.writeJPG(fs);
	nlinfo("Screenshot '%s' saved", filename.c_str());	
}

void C3DTask::render()
{
	CViewport vp;
	CScissor s;
	
	Driver->enableFog(true);
	Scene->render();

	if(C3DTask::getInstance().kbDown(KeyMENU) && C3DTask::getInstance().kbPressed(KeyF2))
		takeScreenShot();
	
	if(EnableExternalCamera && CLevelManager::getInstance().levelPresent() && CLevelManager::getInstance().currentLevel().ExternalCameras.size() > 0)
	{
		CMatrix oldmat = C3DTask::getInstance().scene().getCam().getMatrix();
		
		vp.init(0.69f,0.55f,0.3f,0.3f);
		s.init(0.69f,0.55f,0.3f,0.3f);

		LevelParticle.hide();
		CMatrix m;
		m.identity();
		m.setPos(CLevelManager::getInstance().currentLevel().ExternalCameras[0].first);
		m.setRot(CLevelManager::getInstance().currentLevel().ExternalCameras[0].second);
		C3DTask::getInstance().scene().getCam().setMatrix(m);
		Scene->setViewport(vp);
		Driver->setViewport(vp);
		Driver->setScissor(s);
		Driver->clearBuffers();
		Scene->render();
		LevelParticle.show();

		vp.init(0,0,1,1);
		s.init(0,0,1,1);
		Scene->setViewport(vp);
		Driver->setViewport(vp);
		Driver->setScissor(s);
		C3DTask::getInstance().scene().getCam().setMatrix(oldmat);
	}
	
	C3DTask::getInstance().driver().enableFog(false);
	
	CEntityManager::getInstance().renderNames();
}

void C3DTask::release()
{
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

std::string C3DTask::kbGetString() const
{
	string str = InterfaceListener.line();
	InterfaceListener.setLine("");
	return str;
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
