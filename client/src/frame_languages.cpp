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

static guiSPG<CGuiButton> LanguagesOkButton;
static map<string, guiSPG<CGuiCheck> > Languages;


//
// Classes
//

struct CGuiLangCheckEventBehaviour : public CGuiCheckEventBehaviour
{
	CGuiLangCheckEventBehaviour(const string &varname) : VarName(varname) { }
	virtual ~CGuiLangCheckEventBehaviour() { }
	virtual void onPressed(bool on)
	{
		if(on)
		{
			// save the selected language
			string cf = CConfigFileTask::instance().tempDirectory()+"lang.txt";
			FILE *fp = fopen(cf.c_str(), "wb");
			if(fp)
			{
				fprintf(fp, VarName.c_str());
				fclose(fp);
			}
			for(map<string, guiSPG<CGuiCheck> >::iterator it = Languages.begin(); it != Languages.end(); it++)
			{
				if((*it).first != VarName)
				{
					(*it).second->checked(false);
				}
			}
		}
		else
		{
			Languages[VarName]->checked(true);
		}
	}
private:
	string VarName;
};


//
// Functions
//

static void updateFrameLanguages()
{
	if(LanguagesOkButton->pressed())
	{
		ReloadInterfaces = true;
	}
}

void loadFrameLanguages()
{
	guiSPG<CGuiXml> xml = CGuiObjectManager::instance().loadFrame("languages", updateFrameLanguages);

	LanguagesOkButton = (CGuiButton *)xml->get("bOk");

	// Get available languages
	Languages.clear();
	std::vector<std::string> files;
	CPath::getFileList("uxt", files);

	guiSPG<CGuiVBox> langList = (CGuiVBox*)xml->get("langList");

	for(uint i = 0; i < files.size(); i++)
	{
		string name = CFile::getFilenameWithoutExtension(files[i]);

		Languages[name] = new CGuiCheck;
		Languages[name]->checked(CI18N::getCurrentLanguageCode() == name);
		Languages[name]->eventBehaviour = new CGuiLangCheckEventBehaviour(name);

		guiSPG<CGuiHBox> header = new CGuiHBox;
		header->alignment(CGuiObject::eAlignLeft|CGuiObject::eAlignCenterVertical);
		header->elements.push_back(Languages[name]);
		ucstring longname;
		if(CConfigFileTask::instance().configFile().exists(name))
		{
			longname.fromUtf8(CConfigFileTask::instance().configFile().getVar(name).asString());
			header->elements.push_back(new CGuiText(longname, false));
			langList->elements.push_back(header);
		}
	}
}
