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
// This is the main class that manages all other classes
//

#ifndef MTPT_SOUND_MANAGER_H
#define MTPT_SOUND_MANAGER_H


//
// Includes
//
#include <vector>

#include <nel/misc/singleton.h>
#include <nel/sound/u_audio_mixer.h>
#include <nel/sound/u_source.h>

//
// Classes
//

struct EntitySource
{
	EntitySource(NLSOUND::USource *newsource)
	{
		source = newsource;
		start = true;
		nlinfo("-----------vytvarim zvuk %d", (long int)newsource);
	}

	~EntitySource()
	{
		nlinfo("-----------uvolnuju zvuk %d", (long int)source);
		delete source;
	}

	NLSOUND::USource *source;
	bool start;
};


class CSoundManager : public NLMISC::CSingleton<CSoundManager>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	void updateListener(const NLMISC::CVector &position, const NLMISC::CVector &velocity, const NLMISC::CVector &atVector, const NLMISC::CVector &upVector);
	
	virtual std::string name() const { return "CSoundManager"; }


	enum TSound { BallOpen, BallClose, Splash, Impact, SoundCount };

	EntitySource *playSound(TSound soundID);
	NLSOUND::USource *createTTSource(TSound soundID);
	
	// gui sounds
	void playGUISound(std::string soundName);

	// music
	void loadPlayList();
	void playNextMusic();
	void playPreviousMusic();
	void switchPauseMusic();
	void play();

	void setMusicVolume(float volume);
	void setSoundVolume(float volume);

	friend class NLMISC::CSingleton<CSoundManager>;
	CSoundManager();

private:
	NLSOUND::UAudioMixer *AudioMixer;
	bool PlaySound;
	bool useM3U; // classic music or m3u playlist
	std::vector<std::string> m3uVector; // for holding playlist at runtime
	size_t m3uNowPlaying; // number of now playing file
	enum TMusicState {Stoped, Playing, Paused} musicState;
	float MusicVolume;
	float SoundVolume;
	bool isInit;
};

#endif
