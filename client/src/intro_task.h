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

#ifndef MTPT_INTRO_TASK_H
#define MTPT_INTRO_TASK_H


//
// Includes
//

#include "gui.h"


//
// Classes
//

class CIntroTask : public NLMISC::CSingleton<CIntroTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	void error(std::string &reason);
	void reset();

	void updateMenu();
	void updateInit();
	void updateLoginOnline();
	void updateLoginOnlan();
	void updateServerList();
	void updateConnectionOnLine();
	void updateConnectionOnLan();
	
	virtual std::string name() const { return "CIntroTask"; }
		
	void doConnectionOnLine(uint32 serverId);
	void doConnectionOnLan();

private:

	enum TState { eMenu, eInit, eLoginOnline, eLoginOnlan, eServerList, eConnectionOnline, eConnectionOnlan, eNone };
	
	TState State;

	uint32 ServerId;

	guiSPG<CGuiFrame> testFrame;
	
	guiSPG<CGuiFrame> menuFrame;
	guiSPG<CGuiButton> howToPlay;
	guiSPG<CGuiButton> playOnLineButton;
	guiSPG<CGuiButton> playOnLanButton;
	guiSPG<CGuiButton> exitButton3;
	
	guiSPG<CGuiFrame> loginFrame;
	guiSPG<CGuiText> loginText;
	guiSPG<CGuiText> passwordText;
	guiSPG<CGuiButton> loginButton;
	guiSPG<CGuiButton> backButton1;
	guiSPG<CGuiButton> serverListBackButton;
	guiSPG<CGuiListView> serverListView;
	
	guiSPG<CGuiFrame> loginLanFrame;
	guiSPG<CGuiText> loginLanText;
	guiSPG<CGuiText> passwordLanText;
	guiSPG<CGuiText> serverLanText;
	guiSPG<CGuiButton> loginLanButton;
	guiSPG<CGuiButton> backLanButton1;
	
	guiSPG<CGuiFrame> serverListFrame;
	guiSPG<CGuiVBox> serverVbox;
	guiSPG<CGuiButton> backButton2;
	
	std::string		Text1, Text2, Error1, Error2;
	guiSPG<CGuiFrame> _errorServerFrame;
	uint32 _autoLogin;


};

#endif
