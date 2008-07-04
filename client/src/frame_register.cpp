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

extern string							CryptedPassword;
extern guiSPG<CGuiText>		LoginLoginText;
extern guiSPG<CGuiText>		LoginPasswordText;
extern guiSPG<CGuiButton>	LoginRegisterButton;

static guiSPG<CGuiText>		RegisterLoginText;
static guiSPG<CGuiText>		RegisterPasswordText;
static guiSPG<CGuiText>		RegisterPasswordText2;
static guiSPG<CGuiText>		RegisterEmailText;
static guiSPG<CGuiButton>		RegisterBackButton;
static guiSPG<CGuiButton>		RegisterCreateButton;
static guiSPG<CGuiCheck>		RegisterAcceptCheck;


//
// Functions
//

static void updateFrameRegister()
{
	if(RegisterBackButton->pressed())
	{
		CIntroTask::instance().displayLoginFrame();
	}

	if(RegisterCreateButton->pressed())
	{
		if(!RegisterAcceptCheck->checked())
		{
			error(ucstring("GuiCheckConditions"));
			return;
		}

		string file=CConfigFileTask::instance().tempDirectory()+"reg.tmp";
		string url("http://");
		//url += (CConfigFileTask::instance().configFile().getVar("LSHost").asString() == "localhost")?"localhost/www":"www.mtp-target.org";
		url += "www.mtp-target.org";
		url += "/ucp.php?mode=register&language="+CI18N::getCurrentLanguageCode();
		downloadFile(url+
			"&lang="+CI18N::getCurrentLanguageCode()+
			"&tz=0"+
			"&username="+RegisterLoginText->text().toUtf8()+
			"&email="+RegisterEmailText->text().toUtf8()+
			"&email_confirm="+RegisterEmailText->text().toUtf8()+
			"&new_password="+RegisterPasswordText->text().toUtf8()+
			"&password_confirm="+RegisterPasswordText2->text().toUtf8()+
			"&ig=1", file, true);
		FILE *fp = fopen(file.c_str(), "rb");
		if(!fp)
		{
			error(ucstring("GuiRegisterError"));
			return;
		}
		uint32 size = CFile::getFileSize(fp);
		string fc;
		fc.resize(size+1);
		fread(&fc[0], 1, size, fp);
		fclose(fp);
		NLMISC::CFile::deleteFile(file);

		if(fc.find("<!--REG_OK-->") != string::npos)
		{
			// add one page view for the register
			CNetworkTask::instance().sendGoogleAnalytics("/register/ingame");

			// save the login/pass in the cfg
			CConfigFileTask::instance().configFile().getVar("Login").setAsString(RegisterLoginText->text().toUtf8());
			string pass = RegisterPasswordText->text().toUtf8();
			CHashKeyMD5 hk = getMD5((uint8*)pass.c_str(), pass.size());
			CryptedPassword = hk.toString();
			CConfigFileTask::instance().configFile().getVar("Password").setAsString(CryptedPassword);
			CConfigFileTask::instance().configFile().save();

			// registration success, remove the register button, set the login&pass values and redirect
			LoginLoginText->text(RegisterLoginText->text());
			LoginPasswordText->text(RegisterPasswordText->text());
			LoginRegisterButton->visible(false);
			CIntroTask::instance().displayLoginFrame();
			error(ucstring("GuiRegisterOK"));
			return;
		}

		size_t begin = fc.find("<dd class=\"error\">");
		size_t end = fc.find("</dd>", begin);
		if(begin == string::npos || end == string::npos)
		{
			error(ucstring("GuiRegisterError"));
			return;
		}
		fc = fc.substr(begin+18, end-(begin+18));
		//nlinfo("aaa %s", fc.c_str());

		while(true)
		{
			size_t p = fc.find("<br />");
			if(p == string::npos) break;
			fc.replace(p, 6, "\n");
		}
		//nlinfo("bbb %s", fc.c_str());
		error(ucstring::makeFromUtf8(fc), false);
	}
}

void loadFrameRegister()
{
	guiSPG<CGuiXml> xml = CGuiObjectManager::instance().loadFrame("register", updateFrameRegister);

	RegisterBackButton = (CGuiButton *)xml->get("bBack");
	RegisterCreateButton = (CGuiButton *)xml->get("bCreate");
	RegisterLoginText = (CGuiText*)xml->get("loginEntry");
	RegisterPasswordText = (CGuiText*)xml->get("passwordEntry");
	RegisterPasswordText2 = (CGuiText*)xml->get("passwordEntry2");
	RegisterEmailText = (CGuiText*)xml->get("emailEntry");
	RegisterAcceptCheck = (CGuiCheck *)xml->get("acceptCheck");

	string url("http://");
	//url += (CConfigFileTask::instance().configFile().getVar("LSHost").asString() == "localhost")?"localhost/www":"www.mtp-target.org";
	url += "www.mtp-target.org";
	url += "/ucp.php?mode=register&language="+CI18N::getCurrentLanguageCode();
	((CGuiButton *)xml->get("bReadCondition"))->eventBehaviour = new CGuiOpenUrlButtonEventBehaviour(url);

	/*	RegisterLoginText->text(ucstring("ttttt"));
	RegisterPasswordText->text(ucstring("ttttt"));
	RegisterPasswordText2->text(ucstring("ttttt"));
	RegisterEmailText->text(ucstring("ttttt@tttt.com"));*/
}
