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

#ifndef MTP_TARGET_H
#define MTP_TARGET_H


//
// Includes
//

#include <nel/misc/quat.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/config_file.h>
#include <nel/misc/reader_writer.h>

#include "controler.h"
#include "../../common/constant.h"


//
// Variables
//

extern string ReleaseVersion;	// 1.5.XX
extern uint16 CurrentVersion;	// XX
extern uint16 LatestVersion;	// XX

extern uint8 FundRaised;	// 0-100
extern ucstring FundGoal;

extern uint8 DisplayDebug;
extern bool FollowEntity;
extern NLMISC::CQuat ControlerFreeLookRot;
extern CVector ControlerFreeLookPos;
extern CMatrix ControlerCamMatrix;
extern string ReplayFile;
extern sint32 AutoServerId;
extern string SessionString;
extern bool FullVersion;


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
	void error(const string &reason);
	bool error();

	virtual string name() const { return "CMtpTarget"; }

	CControler		&controler() const { nlassert(Controler); return *Controler; }

	// called by main thread
	void	loadNewSession(bool firstSession);

	string	loadReplayFile();

	// called by the net thread
	void	startSession(float timeBeforeSessionStart, float timeBeforeTimeout, const string &levelName, bool teamMode, const ucstring &str1, const ucstring &str2, bool firstSession=false);
	void	resetFollowedEntity();

	float	timeBeforeSessionStart() const { return TimeBeforeSessionStart; }
	float	timeBeforeTimeout() const { return TimeBeforeTimeout; }
	void	timeBeforeTimeout(float t) { TimeBeforeTimeout = t; }

	enum TState { eLoginPassword, eConnect, eBeforeFirstSession, eStartSession, eReady, eGame, eEndSession };

	TState			State;
	ucstring		String1, String2;
	bool			TeamMode;

	void displayTutorialInfo(bool b);
	bool displayTutorialInfo();

	bool moveReplay();
	void moveReplay(bool b);

	// return true if the player that is running this client is a spectator
	bool spectator();

	bool firstSession() const { return FirstSession; }

private:

	CControler		*Controler;

	bool			FirstSession;

	bool			NewSession;
	float			TimeBeforeSessionStart;
	float			TimeBeforeTimeout;
	string			NewLevelName;
	bool			DoError;

	void			_error();
	string			ErrorReason;
	bool			DisplayTutorialInfo;
	bool			Error;
	bool			MoveReplay;
};

// Convert a string like "BestTime|parm1|param2" with the param replacement using CI18N
ucstring convertVariableString(const ucstring &str);





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


class mtpTarget : public NLMISC::CSingleton<mtpTarget>
{
public:

	//void	init();
	//void	update();
	//void	render();
	//void	renderInterface ();
	//void	release ();

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
};

#endif
