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
// Macros
//

#define ADD_CHECK_VAR(name) \
	static guiSPG<CGuiCheck> Settings##name##Check; \
	Settings##name##Check = (CGuiCheck *)xml->get(#name"Check"); \
	Settings##name##Check->checked(CConfigFileTask::instance().configFile().getVar(#name).asInt() == 1); \
	Settings##name##Check->eventBehaviour = new CGuiCfgCheckEventBehaviour(#name);


//
// Variables
//

static guiSPG<CGuiText>		SettingsResolutionText;
static guiSPG<CGuiHScale>	SettingsResolutionHScale;
static guiSPG<CGuiButton>	SettingsSaveButton;
static vector<UDriver::CMode> modes;


//
// Classes
//


struct CGuiMusicScaleEventBehaviour : public CGuiScaleEventBehaviour
{
	CGuiMusicScaleEventBehaviour() { }
	virtual ~CGuiMusicScaleEventBehaviour() { }
	virtual void onChanged(float value)
	{
		CSoundManager::instance().setMusicVolume(value);
		CConfigFileTask::instance().configFile().getVar("MusicVolume").setAsFloat(value);
	}
};

struct CGuiSoundScaleEventBehaviour : public CGuiScaleEventBehaviour
{
	CGuiSoundScaleEventBehaviour() : chan(0) { }
	virtual ~CGuiSoundScaleEventBehaviour() { }
	virtual void onChanged(float value)
	{
		if(CGuiObjectManager::instance().empty())
			return;

		CSoundManager::instance().setSoundVolume(value);
		CConfigFileTask::instance().configFile().getVar("SoundVolume").setAsFloat(value);
		bool playing = false;
#ifdef USE_FMOD
		if(chan) chan->isPlaying(&playing);
		if(!playing)
		{
			chan = CSoundManager::instance().playSound(CSoundManager::Ready5);
		}
#endif
	}
private:
	TChannel chan;
};

struct CGuiLagCompensationScaleEventBehaviour : public CGuiScaleEventBehaviour
{
	CGuiLagCompensationScaleEventBehaviour() { }
	virtual ~CGuiLagCompensationScaleEventBehaviour() { }
	virtual void onChanged(float value)
	{
		CConfigFileTask::instance().configFile().getVar("NbInterpolatorKeys").setAsInt(int(2.0f+value*2.0f));
	}
};

struct CGuiCfgCheckEventBehaviour : public CGuiCheckEventBehaviour
{
	CGuiCfgCheckEventBehaviour(const string &varname) : VarName(varname) { }
	virtual ~CGuiCfgCheckEventBehaviour() { }
	virtual void onPressed(bool on) { CConfigFileTask::instance().configFile().getVar(VarName).setAsInt(on?1:0); }
private:
	string VarName;
};

struct ModeSort
{
	bool operator()(const UDriver::CMode &m1, const UDriver::CMode &m2)
	{
		return m1.Width*m1.Height < m2.Width*m2.Height;
	}
};

struct CGuiScreenResolutionScaleEventBehaviour : public CGuiScaleEventBehaviour
{
	CGuiScreenResolutionScaleEventBehaviour() { }
	virtual ~CGuiScreenResolutionScaleEventBehaviour() { }
	virtual void onChanged(float value)
	{
		fillModes();
		if(modes.empty()) return;

		int idx = int(value*(modes.size()-1));
		SettingsResolutionText->text(ucstring(toString("%dx%d", modes[idx].Width, modes[idx].Height)));
		CConfigFileTask::instance().configFile().getVar("ScreenWidth").setAsInt(modes[idx].Width);
		CConfigFileTask::instance().configFile().getVar("ScreenHeight").setAsInt(modes[idx].Height);
	}

	static void findCurrentMode()
	{
		fillModes();
		if(modes.empty()) return;

		UDriver::CMode cm;
		C3DTask::instance().driver().getCurrentScreenMode(cm);
		int idx = 0;
		if(!cm.Windowed)
		{
			for(vector<UDriver::CMode>::iterator it=modes.begin();it!=modes.end();it++,idx++)
			{
				UDriver::CMode m = *it;
				if(m.Width == cm.Width && m.Height == cm.Height && m.Depth == cm.Depth && m.Frequency == cm.Frequency)
					break;
			}
		}
		else
		{
			sint32 w = CConfigFileTask::instance().configFile().getVar("ScreenWidth").asInt();
			sint32 h = CConfigFileTask::instance().configFile().getVar("ScreenHeight").asInt();
			for(vector<UDriver::CMode>::iterator it=modes.begin();it!=modes.end();it++,idx++)
			{
				UDriver::CMode m = *it;
				if(m.Width == w && m.Height == h)
					break;
			}
		}
		if(idx != modes.size())
		{
			SettingsResolutionHScale->percent((float(idx)+0.5f)/float(modes.size()-1));
		}
	}

private:

	static void fillModes()
	{
		if(!modes.empty()) return;

		static vector<UDriver::CMode> allmodes;
		C3DTask::instance().driver().getModes(allmodes);

		// Add the 800x600 mode if no mode was found
		if(allmodes.empty()) allmodes.push_back(UDriver::CMode(800, 600, 32));

		set<string> goodmodes;

		for(vector<UDriver::CMode>::iterator it=allmodes.begin();it!=allmodes.end();it++)
		{
			if((*it).Width >= 800 && (*it).Height >= 600)
				goodmodes.insert(toString("%d|%d",(*it).Width,(*it).Height));
		}

		for(set<string>::iterator it=goodmodes.begin();it!=goodmodes.end();it++)
		{
			vector<string> line;
			explode(*it, string("|"), line);
			modes.push_back(UDriver::CMode(atoi(line[0].c_str()),atoi(line[1].c_str()),0));
		}

		sort(modes.begin(), modes.end(), ModeSort());

		nlinfo("Available video mode:");
		if(modes.empty()) nlinfo("   no video mode...");
		for(vector<UDriver::CMode>::iterator it=modes.begin();it!=modes.end();it++)
		{
			UDriver::CMode m = *it;
			nlinfo("   > %dx%d",m.Width,m.Height);
		}
	}
};


//
// Functions
//

static void updateFrameSettings()
{
	if(SettingsSaveButton->pressed())
	{
		CConfigFileTask::instance().configFile().save();
		CIntroTask::instance().displayLoginFrame();
	}
}

void loadFrameSettings()
{
	guiSPG<CGuiXml> xml = CGuiObjectManager::instance().loadFrame("settings", updateFrameSettings);

	((CGuiButton *)xml->get("bLanguages"))->eventBehaviour = new CGuiDisplayFrameEventBehaviour("languages");

	SettingsResolutionText = (CGuiText *)xml->get("resolutionText");
	SettingsResolutionHScale = (CGuiHScale *)xml->get("resolutionHScale");
	SettingsResolutionHScale->eventBehaviour = new CGuiScreenResolutionScaleEventBehaviour;
	SettingsSaveButton = (CGuiButton *)xml->get("bSave");
	CGuiScreenResolutionScaleEventBehaviour::findCurrentMode();

	static float SettingsMusicVolumePercent = 0.8f;
	CGuiHScale *hscale = (CGuiHScale *)xml->get("MusicVolumeHScale");
	hscale->ptrValue(&SettingsMusicVolumePercent);
	hscale->eventBehaviour = new CGuiMusicScaleEventBehaviour;
	hscale->percent(CConfigFileTask::instance().configFile().getVar("MusicVolume").asFloat());
	CGuiTextPercent *stext = (CGuiTextPercent *)xml->get("MusicVolumePercentText");
	stext->ptrValue(&SettingsMusicVolumePercent);

	static float SettingsSoundVolumePercent = 1.0f;
	hscale = (CGuiHScale *)xml->get("SoundVolumeHScale");
	hscale->ptrValue(&SettingsSoundVolumePercent);
	hscale->eventBehaviour = new CGuiSoundScaleEventBehaviour;
	hscale->percent(CConfigFileTask::instance().configFile().getVar("SoundVolume").asFloat());
	stext = (CGuiTextPercent *)xml->get("SoundVolumePercentText");
	stext->ptrValue(&SettingsSoundVolumePercent);

	ADD_CHECK_VAR(Fullscreen);
	ADD_CHECK_VAR(VSync);
	ADD_CHECK_VAR(AntiAlias);
	ADD_CHECK_VAR(DisplayClouds);

	ADD_CHECK_VAR(Music);
	ADD_CHECK_VAR(Sound);

	ADD_CHECK_VAR(UseUdp);

	ADD_CHECK_VAR(LogChat);

	static float SettingsLagCompensationPercent = 1.0f;
	hscale = (CGuiHScale *)xml->get("LagCompensationHScale");
	hscale->ptrValue(&SettingsLagCompensationPercent);
	hscale->eventBehaviour = new CGuiLagCompensationScaleEventBehaviour;
	hscale->percent((float(CConfigFileTask::instance().configFile().getVar("NbInterpolatorKeys").asInt())-2.0f)/2.0f);
	stext = (CGuiTextPercent *)xml->get("LagCompensationPercentText");
	stext->ptrValue(&SettingsLagCompensationPercent);
}
