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

#include <fstream>
#include <iostream>


#include "hud_task.h"
#include "sound_manager.h"
#include "config_file_task.h"

#ifdef USE_FMOD
#	pragma comment(lib, "fmodex_vc")
// debug: generate fmod log
//#	pragma comment(lib, "fmodexL_vc")
#endif


//
// Namespaces
//

using namespace NLMISC;


//
// Variables
//

#ifdef USE_FMOD

struct CSound
{
	char *Name;
	FMOD_MODE Mode;
	FMOD::Sound *Sound;
};

CSound Sounds[] = {
	{ "0.wav", FMOD_2D, 0 },
	{ "1.wav", FMOD_2D, 0 },
	{ "2.wav", FMOD_2D, 0 },
	{ "3.wav", FMOD_2D, 0 },
	{ "4.wav", FMOD_2D, 0 },
	{ "5.wav", FMOD_2D, 0 },
	{ "open.wav", FMOD_3D, 0 },
	{ "close.wav", FMOD_3D, 0 },
	{ "splash.wav", FMOD_3D, 0 },
	{ "impact.wav", FMOD_3D, 0 },
};


#endif // USE_FMOD


//
// Functions
//

#ifdef USE_FMOD

#define FMOD_CHECKR(fct, ret) \
{ \
	FMOD_RESULT result = fct; \
	if (result != FMOD_OK) \
	{ \
		nlwarning("FMOD: error(%d) %s\n", result, FMOD_ErrorString(result)); \
		ret; \
	} \
}

#define FMOD_CHECK(fct) \
{ \
	FMOD_RESULT result = fct; \
	if (result != FMOD_OK) \
	{ \
		nlwarning("FMOD: error(%d) %s\n", result, FMOD_ErrorString(result)); \
	} \
}

#endif

CSoundManager::CSoundManager()
{
#ifdef USE_FMOD
	IsInit = false;
	System = 0;
	PlayListIndex = 0;
	MusicSound = 0;
	MusicChannel = 0;
	PlaySound = false;
#endif
}

void CSoundManager::init()
{
#ifdef USE_FMOD
	// no music and no sound? no need to init fmod
	if (CConfigFileTask::instance().configFile().getVar("Sound").asInt() == 0 && CConfigFileTask::instance().configFile().getVar("Music").asInt() == 0)
		return;

// debug: generate fmod log
// 	FMOD_DEBUGLEVEL dbg;
// 	FMOD_CHECK(FMOD::Debug_GetLevel(&dbg));
// 	FMOD_CHECK(FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_ALL | FMOD_DEBUG_DISPLAY_ALL | FMOD_DEBUG_TYPE_ALL));

	FMOD_CHECKR(FMOD::System_Create(&System), return);

	uint version;
	FMOD_CHECKR(System->getVersion(&version), return);

	if (version < FMOD_VERSION)
	{
		nlwarning("FMOD error: You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		return;
	}

	//doesn't help to fix the problem of sound on linux/macoook System->setStreamBufferSize(16384*5, FMOD_TIMEUNIT_RAWBYTES);

	FMOD_CHECKR(System->init(20, FMOD_INIT_NORMAL, 0), return);

	if (CConfigFileTask::instance().configFile().getVar("Sound").asInt() == 1)
	{
		nlassert(sizeof(Sounds)/sizeof(Sounds[0]) == SoundCount);
		for(uint i = 0; i < SoundCount; i++)
		{
			FMOD_CHECK(System->createSound(CPath::lookup(Sounds[i].Name).c_str(), Sounds[i].Mode, 0, &Sounds[i].Sound));
			if(Sounds[i].Mode == FMOD_3D)
			{
				//FMOD_CHECK(Sounds[i].Sound->setMode(FMOD_LOOP_NORMAL));
				FMOD_CHECK(Sounds[i].Sound->set3DMinMaxDistance(3.0f, 10000.0f));
			}
		}
		PlaySound = true;
	}

	setMusicVolume(CConfigFileTask::instance().configFile().getVar("MusicVolume").asFloat());
	setSoundVolume(CConfigFileTask::instance().configFile().getVar("SoundVolume").asFloat());

	loadPlayList();

	IsInit = true;

	playPlayList(false);

#endif
}

void CSoundManager::update()
{
#ifdef USE_FMOD
	if(!IsInit) return;

	FMOD_CHECK(System->update());
#endif
}

void CSoundManager::render()
{
}

void CSoundManager::release()
{
#ifdef USE_FMOD
	if(!IsInit) return;

	stopPlayList();
	FMOD_CHECK(System->release());
#endif
}

void CSoundManager::updateListener(const NLMISC::CVector &position, const NLMISC::CVector &velocity, const NLMISC::CVector &atVector, const NLMISC::CVector &upVector)
{
#ifdef USE_FMOD
	if(!IsInit || !PlaySound) return;

	FMOD_VECTOR p, a, u;
	// fmod is left handed
	p.x = position.x/GScale;
	p.y = position.y/GScale;
	p.z = -position.z/GScale;

	a.x = atVector.x;
	a.y = atVector.y;
	a.z = -atVector.z;

	u.x = upVector.x;
	u.y = upVector.y;
	u.z = -upVector.z;

	FMOD_CHECK(System->set3DListenerAttributes(0, &p, 0, &a, &u));
#endif
}

void CSoundManager::playNextMusic()
{
#ifdef USE_FMOD
	if(!IsInit) return;
	if(PlayList.size() == 1) return;
	stopPlayList();
	nextPlayList();
	playPlayList();
#endif
}

void CSoundManager::playPreviousMusic()
{
#ifdef USE_FMOD
	if(!IsInit) return;
	if(PlayList.size() == 1) return;
	stopPlayList();
	previousPlayList();
	playPlayList();
#endif
}

void CSoundManager::switchPauseMusic()
{
#ifdef USE_FMOD
	if(!IsInit) return;
	if(MusicChannel == 0) return;

	bool paused;
	FMOD_CHECKR(MusicChannel->getPaused(&paused), return);
	FMOD_CHECK(MusicChannel->setPaused(!paused));
#endif
}

TChannel CSoundManager::playSound(TSound sound)
{
#ifdef USE_FMOD
	if(!IsInit || !PlaySound) return 0;

	FMOD::Channel *channel = 0;
	FMOD_CHECK(System->playSound(FMOD_CHANNEL_FREE, Sounds[sound].Sound, false, &channel));
	FMOD_CHECK(channel->setVolume(SoundVolume));
	return channel;
#endif
}

#ifdef USE_FMOD

void CSoundManager::loadPlayList()
{
	if (CConfigFileTask::instance().configFile().getVar("Music").asInt() != 1) return;

	string m3uFileName = CConfigFileTask::instance().configFile().getVar("M3UList").asString();

	if(!CFile::fileExists(m3uFileName) && CPath::exists(CFile::getFilename(m3uFileName)))
	{
		m3uFileName = CPath::lookup(CFile::getFilename(m3uFileName));
	}

	string m3uDirectory = CFile::getPath(m3uFileName);
	nlinfo("Loading playlist '%s'", m3uFileName.c_str());
	ifstream M3uFile;
	M3uFile.open(m3uFileName.c_str());
	if (!M3uFile.good())
	{
		nlwarning("Can't load playlist '%s'", m3uFileName.c_str());
		return;
	}

	while (!M3uFile.eof() && M3uFile.good())
	{
		string cline;
		getline(M3uFile, cline);
		if(cline.size() > 0 && cline[0] != '#' && cline[0] != ' ')
		{
			while(cline.size() > 0 && (cline[cline.size()-1] == '\n' || cline[cline.size()-1] == '\r')) cline.resize(cline.size()-1)
				;

			if (CFile::isExists(cline))
			{
				nlinfo("Adding music '%s' in the playlist", cline.c_str());
				PlayList.push_back(cline);
			}
			else if (CFile::isExists(m3uDirectory+cline))
			{
				nlinfo("Adding music '%s' in the playlist", (m3uDirectory+cline).c_str());
				PlayList.push_back(m3uDirectory+cline);
			}
			else
			{
				nlwarning("Music '%s' is not found", cline.c_str());
			}
		}
	}
	if (CConfigFileTask::instance().configFile().getVar("M3UShuffle").asInt() == 1)
	{
		srand (CTime::getSecondsSince1970());
		for(uint32 i = 0; i < PlayList.size(); i++)
		{
			uint32 j = rand()%PlayList.size();
			string tmp = PlayList[j];
			PlayList[j] = PlayList[i];
			PlayList[i] = tmp;
		}
	}
}

// get called when music ends
static FMOD_RESULT F_CALLBACK MusicEndCallback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2)
{
	CSoundManager::instance().playNextMusic();
	return FMOD_OK;
}

void CSoundManager::playPlayList(bool displayTitle)
{
	if(!IsInit) return;
	if(PlayListIndex >= PlayList.size()) return;

	string longName = PlayList[PlayListIndex];
	if(PlayList.size() == 1)
	{
		FMOD_CHECKR(System->createStream(longName.c_str(), FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &MusicSound), return);
	}
	else
	{
		FMOD_RESULT res = System->createStream(longName.c_str(), FMOD_DEFAULT, 0, &MusicSound);
		if(res == FMOD_ERR_FORMAT)
		{
			playNextMusic();
			return;
		}
		else
		{
			FMOD_CHECKR(res, return);
		}
	}

	FMOD_CHECKR(System->playSound(FMOD_CHANNEL_FREE, MusicSound, false, &MusicChannel), return);
	FMOD_CHECK(MusicChannel->setVolume(MusicVolume));
	if(PlayList.size() > 1) FMOD_CHECK(MusicChannel->setCallback(FMOD_CHANNEL_CALLBACKTYPE_END, MusicEndCallback, 0));

	if(displayTitle) CHudTask::instance().addSysMessage(CI18N::get("NowPlaying")+" "+CFile::getFilename(longName));
}

void CSoundManager::stopPlayList()
{
	if(!IsInit) return;

	if(MusicChannel != 0)
	{
		//FMOD_CHECK(MusicChannel->setCallback(FMOD_CHANNEL_CALLBACKTYPE_END, 0, 0));
		//FMOD_CHECK(MusicChannel->stop());
		MusicChannel = 0;
	}

	if(MusicSound != 0)
	{
		FMOD_CHECK(MusicSound->release());
		MusicSound = 0;
	}
}

void CSoundManager::nextPlayList()
{
	if(PlayList.size() < 2) return;
	PlayListIndex = (PlayListIndex+1)%PlayList.size();
}

void CSoundManager::previousPlayList()
{
	if(PlayList.size() < 2) return;
	if(PlayListIndex == 0)
		PlayListIndex = PlayList.size()-1;
	else
		PlayListIndex--;
}

#endif

void CSoundManager::setMusicVolume(float volume)
{
#ifdef USE_FMOD
	MusicVolume = volume;
#endif
}

void CSoundManager::setSoundVolume(float volume)
{
#ifdef USE_FMOD
	SoundVolume = volume;
#endif
}
