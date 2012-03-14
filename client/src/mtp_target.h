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

#ifndef MTP_TARGET_H
#define MTP_TARGET_H

#define MTPT_RELEASE_VERSION_NUMBER "1.2.2a"
#define MTPT_RELEASE_VERSION_NAME ""


//
// Includes
//

#include <nel/misc/config_file.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/quat.h>
#include <nel/misc/reader_writer.h>

#include "global.h"
#include "controler.h"
#include "interface.h"


//
// Variables
//

extern bool DisplayDebug;
extern bool FollowEntity;
extern NLMISC::CQuat ControlerFreeLookRot;
extern CVector ControlerFreeLookPos;
extern CMatrix ControlerCamMatrix;
extern std::string ReplayFile;
extern sint32 AutoServerId;


//
// Classes
//


class CMtpTarget : public NLMISC::CSingleton<CMtpTarget>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render() { }
	virtual void release() { }

	void reset();
	void error(const std::string &reason);
	bool error();
	
	virtual std::string name() const { return "CMtpTarget"; }

	CControler		&controler() const { nlassert(Controler); return *Controler; }

	// called by main thread
	void	loadNewSession();

	std::string	loadReplayFile();

	// called by the net thread
	void	startSession(float timeBeforeSessionStart, float timeBeforeTimeout, const std::string &levelName, const std::string &str1, const std::string &str2,bool isSpectatorOnly=false);
	void	resetFollowedEntity();

	float	timeBeforeSessionStart() const { return TimeBeforeSessionStart; }
	float	timeBeforeTimeout() const { return TimeBeforeTimeout; }
	void	timeBeforeTimeout(float t) { TimeBeforeTimeout = t; }
	
	enum TState { eLoginPassword, eConnect, eBeforeFirstSession, eStartSession, eReady, eGame, eEndSession };
	
	TState State;
	std::string		String1, String2;
	
	void displayTutorialInfo(bool b);
	bool displayTutorialInfo();

	bool moveReplay();
	void moveReplay(bool b);
	std::string	sessionFileName() {return SessionFileName;}
	
	bool isSpectatorOnly() {return IsSpectatorOnly;};
private:

	CControler		*Controler;
	
	bool			NewSession;
	bool			IsSpectatorOnly;
	float			TimeBeforeSessionStart;
	float			TimeBeforeTimeout;
	std::string		NewLevelName;
	bool			DoError;
	
	void _error();
	std::string		ErrorReason;
	std::string		SessionFileName;
	bool			DisplayTutorialInfo;
	bool			Error;
	bool			MoveReplay;
};







//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


class mtpTarget : public NLMISC::CSingleton<mtpTarget>
{
public:

	void	init();
	void	update();
	void	render();
	//void	renderInterface ();
	void	release ();

	void	everybodyReady();
	void	endSession();
	
//	void	resetFollowedEntity();
	void	toggleQuakeControl();

//	float	getTime () { return Time; }
//	float	getDeltaTime () { return DeltaTime; }

	//mtInterface		 Interface2d;
	
//	static NLMISC::CConfigFile ConfigFile;

		
private:

//	bool loadPhysicLevel(const char *filename);
//	bool loadLevel(const char *filename);
	
	bool	FirstResetCamera;

	float TimeBeforeSessionStart, TimeBeforeTimeout;

	// use accessor to access these values
//	float Time, DeltaTime, OldTime;
//	NLMISC::CValueSmoother DeltaTimeSmooth;
//	double FirstTime;
//	void updateTime ();
//	void updateSound ();

	friend class mtInterface;
	friend class CResourceManager;
};

#endif
