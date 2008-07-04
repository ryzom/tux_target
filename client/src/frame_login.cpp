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
// Variables
//

guiSPG<CGuiText>				LoginLoginText;
guiSPG<CGuiText>				LoginPasswordText;
static guiSPG<CGuiButton>		LoginLoginButton;
string									CryptedPassword;
guiSPG<CGuiButton>				LoginRegisterButton;


//
// Functions
//

ucstring login()
{
	return (LoginLoginText.p==0) ? ucstring("") : LoginLoginText->text();
}

static void updateFrameLogin()
{
	if(LoginLoginButton->pressed() || AutoLogin)
	{
		LoginLoginText->text(LoginLoginText->text());
		LoginPasswordText->text(LoginPasswordText->text());
		CConfigFileTask::instance().configFile().getVar("Login").setAsString(LoginLoginText->text().toUtf8());
		if(LoginPasswordText->text().toUtf8() != "123456789" && LoginPasswordText->text() != ucstring(""))
		{
			// player changed the password in the gui, recompute the new one
			string pass = LoginPasswordText->text().toUtf8();
			CHashKeyMD5 hk = getMD5((uint8*)pass.c_str(), pass.size());
			CryptedPassword = hk.toString();
			CConfigFileTask::instance().configFile().getVar("Password").setAsString(CryptedPassword);
		}
		CConfigFileTask::instance().configFile().save();

		string loginServer = CConfigFileTask::instance().configFile().getVar("LSHost").asString();
		string reason = CLoginClientMtp::authenticate(loginServer, LoginLoginText->text(), CryptedPassword);
		if(!reason.empty())
		{
			AutoLogin = false;
			error(ucstring::makeFromUtf8(reason));
		}
		else
		{
			ServerId = -1;
			loadFrameServerList();
			CGuiObjectManager::instance().displayFrame("server_list");
		}
	}
}

void loadFrameLogin()
{
	guiSPG<CGuiXml> xml = CGuiObjectManager::instance().loadFrame("login", updateFrameLogin);

	LoginLoginText = (CGuiText*)xml->get("loginEntry");
	LoginLoginText->text(ucstring::makeFromUtf8(CConfigFileTask::instance().configFile().getVar("Login").asString()));

	LoginPasswordText = (CGuiText*)xml->get("passwordEntry");
	if(CConfigFileTask::instance().configFile().exists("Password") && CConfigFileTask::instance().configFile().getVar("Password").asString() != "")
	{
		CryptedPassword = CConfigFileTask::instance().configFile().getVar("Password").asString();
		LoginPasswordText->text(ucstring::makeFromUtf8("123456789"));
	}

	LoginLoginButton = (CGuiButton *)xml->get("bLogin");

	LoginRegisterButton = ((CGuiButton *)xml->get("bRegister"));
	LoginRegisterButton->eventBehaviour = new CGuiDisplayFrameEventBehaviour("register");

	if(!LoginLoginText->text().empty())
	{
		LoginRegisterButton->visible(false);
	}

	((CGuiButton *)xml->get("bSettings"))->eventBehaviour = new CGuiDisplayFrameEventBehaviour("settings");
	((CGuiButton *)xml->get("bLostPassword"))->eventBehaviour = new CGuiOpenUrlButtonEventBehaviour("http://www.mtp-target.org/ucp.php?mode=sendpassword&language="+CI18N::getCurrentLanguageCode());
	//((CGuiButton *)xml->get("bHowToPlay"))->eventBehaviour = new CGuiOpenUrlButtonEventBehaviour("http://www.mtp-target.org/viewtopic.php?f=12&t=14");
	((CGuiButton *)xml->get("bHowToPlay"))->eventBehaviour = new CGuiAddFrameEventBehaviour("help1");
	((CGuiButton *)xml->get("bExit"))->eventBehaviour = new CGuiExitButtonEventBehaviour();
}
