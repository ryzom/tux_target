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

#ifndef MT_SOUND_MANAGER_H
#define MT_SOUND_MANAGER_H


//
// Includes
//

#include <nel/misc/singleton.h>


//
// Types
//

#ifdef USE_FMOD
typedef	FMOD::Channel *TChannel;
#else
typedef	void *TChannel;
#endif


//
// Classes
//

class CSoundManager : public NLMISC::CSingleton<CSoundManager>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual string name() const { return "CSoundManager"; }

	// Sound enum
	enum TSound { Ready0, Ready1, Ready2, Ready3, Ready4, Ready5, BallOpen, BallClose, Splash, Impact, SoundCount };

	TChannel playSound(TSound sound);

	void playNextMusic();
	void playPreviousMusic();
	void switchPauseMusic();

	void updateListener(const NLMISC::CVector &position, const NLMISC::CVector &velocity, const NLMISC::CVector &atVector, const NLMISC::CVector &upVector);

	void setMusicVolume(float volume);
	void setSoundVolume(float volume);

private:

	CSoundManager();
	friend class NLMISC::CSingleton<CSoundManager>;

#ifdef USE_FMOD

	FMOD::System *System;
	bool IsInit;

	// Playlist

	void loadPlayList();
	void playPlayList(bool displayTitle = true);
	void nextPlayList();
	void previousPlayList();
	void stopPlayList();

	FMOD::Sound *MusicSound;
	FMOD::Channel *MusicChannel;
	vector<string> PlayList;
	uint32 PlayListIndex; // Index in the PlayList of the current now file that is played

	float MusicVolume;
	float SoundVolume;

	bool PlaySound;

#endif
};

#endif
