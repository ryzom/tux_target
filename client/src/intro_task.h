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

#ifndef MT_INTRO_TASK_H
#define MT_INTRO_TASK_H


//
// Includes
//

#include "gui.h"
#include "task_manager.h"


//
// Variables
//

extern uint32		ServerId;
extern bool		AutoLogin;
extern bool		ReloadInterfaces;


//
// Functions
//

void loadFrameDonation();
void loadFrameError();
void loadFrameHelp();
void loadFrameLanguages();
void loadFrameLogin();
void loadFrameMessage();
void loadFrameRegister();
void loadFrameServerList();
void loadFrameSettings();

ucstring login();
void error(const ucstring &reason, bool convert = true);
void displayMessage(const ucstring &msg, CGuiButtonEventBehaviour *ok = 0, CGuiButtonEventBehaviour *cancel = 0);


//
// Classes
//

class CIntroTask : public NLMISC::CSingleton<CIntroTask>, public ITask
{
public:

	CIntroTask() { }

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release() { }
	virtual string name() const { return "CIntroTask"; }

	void displayLoginFrame();

	void doConnection(uint32 serverId);
	uint32 serverId() const { return ServerId; }
	bool loadLang();
	void loadInterfaces();
};

struct CGuiOpenUrlButtonEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiOpenUrlButtonEventBehaviour(const string &url, bool quit=false) : Url(url), Quit(quit) { }
	virtual ~CGuiOpenUrlButtonEventBehaviour() { }
	virtual void onPressed()
	{
		nlinfo("opening url: '%s'", Url.c_str());
		NLMISC::openURL(Url.c_str());
		if(Quit) CTaskManager::instance().exit();
	}
private:
	string Url;
	bool Quit;
};

struct CGuiExitButtonEventBehaviour : public CGuiButtonEventBehaviour
{
	virtual void onPressed() { CTaskManager::instance().exit(); }
};

struct CGuiRemoveFrameEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiRemoveFrameEventBehaviour(const std::string &frameName) : FrameName(frameName) {}
	virtual void onPressed() { CGuiObjectManager::instance().removeFrame(FrameName); }
private:
	const std::string FrameName;
};

struct CGuiAddFrameEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiAddFrameEventBehaviour(const std::string &frameName) : FrameName(frameName) {}
	virtual void onPressed() { CGuiObjectManager::instance().addFrame(FrameName); }
private:
	const std::string FrameName;
};

struct CGuiDisplayFrameEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiDisplayFrameEventBehaviour(const std::string &frameName) : FrameName(frameName) {}
	virtual void onPressed() { CGuiObjectManager::instance().displayFrame(FrameName); }
private:
	const std::string FrameName;
};

struct CGuiReplaceFrameEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiReplaceFrameEventBehaviour(const std::string &remove, const std::string &add) : Remove(remove), Add(add) {}
	virtual void onPressed() { CGuiObjectManager::instance().removeFrame(Remove); CGuiObjectManager::instance().addFrame(Add); }
private:
	const std::string Remove, Add;
};

#endif
