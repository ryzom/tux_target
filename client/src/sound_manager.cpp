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
#include <string>
#include <fstream>
#include <algorithm>

#include "sound_manager.h"
#include "config_file_task.h"
#include "resource_manager2.h"
#include "3d_task.h"
#include "hud_task.h"

#include <nel/sound/u_listener.h>

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLSOUND;

//
// Variables
//

//
// Functions
//

EntitySource *CSoundManager::playSound (TSound soundID)
{
	if(!isInit || !PlaySound)
		return 0;

	USource *source=createTTSource(soundID);
	if(0==source)
		return 0;

	EntitySource *Pom = new EntitySource(source);
	return Pom;
}

USource *CSoundManager::createTTSource (TSound soundID)
{
	if(soundID==BallOpen)
		return AudioMixer->createSource(CStringMapper::map("open"));
	if(soundID==BallClose)
		return AudioMixer->createSource(CStringMapper::map("close"));
	if(soundID==Splash)
		return AudioMixer->createSource(CStringMapper::map("splash"));
	if(soundID==Impact)
		return AudioMixer->createSource(CStringMapper::map("beep"));

	return 0;
}

CSoundManager::CSoundManager()
{
	isInit = false;
	musicState = Stoped;
	m3uNowPlaying = 0;
	PlaySound = false;
}

void CSoundManager::init()
{

//	CPath::addSearchPath("/home/xnovak5/c/sound_sources/data", true, false);

	/*
	 * 1. Create the audio mixer object and init it.
	 * If the sound driver cannot be loaded, an exception is thrown.
	 */
	AudioMixer = UAudioMixer::createAudioMixer();

//	// Set the sample path before init, this allow the mixer to build the sample banks
//	AudioMixer->setSamplePath("/home/xnovak5/c/sound_sources/data/samplebank");
//	// Packed sheet option, this mean we want packed sheet generated in 'data' folder
//	AudioMixer->setPackedSheetOption("/home/xnovak5/c/sound_sources/data", true);

	UAudioMixer::TDriver driverType;
	string driverName = CConfigFileTask::getInstance().configFile().getVar("SoundDriver").asString();
	if (driverName == "OpenAL") driverType = UAudioMixer::DriverOpenAl;
	else if (driverName == "FMod") driverType = UAudioMixer::DriverFMod;
	else if (driverName == "XAudio2") driverType = UAudioMixer:: DriverXAudio2;
	else if (driverName == "DSound") driverType = UAudioMixer::DriverDSound;
	else
	{
		driverType = UAudioMixer::DriverAuto;
		nlwarning("SoundDriver value '%s' is invalid.", driverName.c_str());
	}

	AudioMixer->init(
		CConfigFileTask::getInstance().configFile().exists("SoundMaxTracks") ? CConfigFileTask::getInstance().configFile().getVar("SoundMaxTracks").asInt() : 32,
		CConfigFileTask::getInstance().configFile().exists("SoundUseEax") ? CConfigFileTask::getInstance().configFile().getVar("SoundUseEax").asBool() : true,
		CConfigFileTask::getInstance().configFile().exists("SoundUseADPCM") ? CConfigFileTask::getInstance().configFile().getVar("SoundUseADPCM").asBool() : false,
		NULL, true, driverType,
		CConfigFileTask::getInstance().configFile().exists("SoundForceSoftware") ? CConfigFileTask::getInstance().configFile().getVar("SoundForceSoftware").asBool() : true);

	if (CConfigFileTask::instance().configFile().getVar("Sound").asInt() == 1)
	{
		PlaySound = true;
	}

	setMusicVolume(CConfigFileTask::instance().configFile().getVar("MusicVolume").asFloat());
	setSoundVolume(CConfigFileTask::instance().configFile().getVar("SoundVolume").asFloat());

	loadPlayList();

	isInit = true;

	play();
}

void CSoundManager::update()
{
	if(!isInit) return;

	const CMatrix &cameraMatrix = C3DTask::instance().scene().getCam().getMatrix();
	updateListener(cameraMatrix.getPos(), CVector::Null, cameraMatrix.getJ(), cameraMatrix.getK());

	AudioMixer->update();

	if(!m3uVector.empty() && AudioMixer->isMusicEnded())
	{
		playNextMusic();
	}
}

void CSoundManager::updateListener(const NLMISC::CVector &position, const NLMISC::CVector &velocity, const NLMISC::CVector &atVector, const NLMISC::CVector &upVector)
{
	if(!isInit) return;

	AudioMixer->getListener()->setPos(position);
	AudioMixer->getListener()->setOrientation(atVector, upVector);
	AudioMixer->getListener()->setVelocity(velocity);
}

void CSoundManager::render()
{
}

void CSoundManager::release()
{
	if(!isInit) return;

	delete AudioMixer;
	AudioMixer = NULL;
}

// -----------------------------------------------------------------------------
// gui methods
// -----------------------------------------------------------------------------

void CSoundManager::playGUISound (string soundName)
{
	if(!isInit || !PlaySound)
		return;

	USource *soundSource = AudioMixer->createSource(CStringMapper::map(soundName), true);
	if (NULL == soundSource)
	{
		nlwarning("----can't find GUI sound: %s", soundName.c_str());
		return;
	}
	soundSource->setSourceRelativeMode(true);
	soundSource->play();
	return;
}

// -----------------------------------------------------------------------------

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
				m3uVector.push_back(cline);
			}
			else if (CFile::isExists(m3uDirectory+cline))
			{
				nlinfo("Adding music '%s' in the playlist", (m3uDirectory+cline).c_str());
				m3uVector.push_back(m3uDirectory+cline);
			}
			else
			{
				nlwarning("Music '%s' is not found", cline.c_str());
			}
		}
	}
	if (CConfigFileTask::instance().configFile().getVar("M3UShuffle").asInt() == 1)
	{
		random_shuffle(m3uVector.begin(), m3uVector.end());
		/*
		srand (CTime::getSecondsSince1970());
		for(uint32 i = 0; i < m3uVector.size(); i++)
		{
			uint32 j = rand()%m3uVector.size();
			string tmp = m3uVector[j];
			m3uVector[j] = m3uVector[i];
			m3uVector[i] = tmp;
		}
		*/
	}
}

// -----------------------------------------------------------------------------

void CSoundManager::playNextMusic()
{
	if (m3uVector.empty())
		return;

	m3uNowPlaying++;
	m3uNowPlaying%=m3uVector.size();
	play();
}

void CSoundManager::playPreviousMusic()
{
	if (m3uVector.empty())
		return;

	if(m3uNowPlaying == 0)
		m3uNowPlaying = m3uVector.size()-1;
	else
		m3uNowPlaying--;

	play();
}

void CSoundManager::switchPauseMusic()
{
	if(!isInit) return;

	if (musicState == Playing)
	{
		AudioMixer->pauseMusic();
		musicState = Paused;
	}
	else if (musicState == Paused )
	{
		AudioMixer->resumeMusic();
		musicState = Playing;
	}
}

void CSoundManager::setMusicVolume(float volume)
{
	MusicVolume = volume;

	if(isInit)
		AudioMixer->setMusicVolume(volume);
}

void CSoundManager::setSoundVolume(float volume)
{
	SoundVolume = volume;

	if(isInit)
		AudioMixer->getListener()->setGain(SoundVolume);
}

void CSoundManager::play ()
{
	if(!isInit)
		return;

	if (!m3uVector.empty())
	{
		nlassert (m3uNowPlaying<m3uVector.size());

		/* If the player is paused, resume, else, play the current song */
		if (musicState == Paused)
		{
			AudioMixer->resumeMusic();
			musicState = Playing;
		}
		else
		{
			if(AudioMixer->playMusic(m3uVector[m3uNowPlaying], 0, true, false))
			{
				string SongTitle;
				if(AudioMixer->getSongTitle(m3uVector[m3uNowPlaying], SongTitle))
				{
					CHudTask::getInstance().addSysMessage("Now playing " + SongTitle );
				}
				musicState = Playing;
			}
			else
			{
				nlwarning("Play music '%s' failed", m3uVector[m3uNowPlaying].c_str());
				vector<string>::iterator it = m3uVector.begin() + m3uNowPlaying;
				m3uVector.erase(it);
				if(m3uNowPlaying >= m3uVector.size())
				{
					m3uNowPlaying=0;
				}
			}
		}
	}
}
