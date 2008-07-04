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

static guiSPG<CGuiButton>		ServerListBackButton;
static guiSPG<CGuiListView>	ServerListView;


//
// Classes
//

struct CGuiServerListConnectButtonEventBehaviour : public CGuiButtonEventBehaviour
{
	CGuiServerListConnectButtonEventBehaviour(CGuiListView *serverListView) : _serverListView(serverListView) { }
	virtual ~CGuiServerListConnectButtonEventBehaviour() { }
	virtual void onPressed()
	{
		int rowId = _serverListView->selectedRow();
		nlinfo("user want to connect to server : %d",rowId);
		if(rowId>=0)
			CIntroTask::instance().doConnection(rowId);
	}
private:
	guiSPG<CGuiListView> _serverListView;
};

class CGuiLaunchServerEventBehaviour : public CGuiListViewEventBehaviour
{
public:
	CGuiLaunchServerEventBehaviour() { }
	virtual ~CGuiLaunchServerEventBehaviour() { }
	virtual void onPressed(int rowId)
	{
		nlinfo("user want to connect to server : %d",rowId);
		if(rowId>=0)
			CIntroTask::instance().doConnection(rowId);
	}
};


//
// Functions
//

static void updateFrameServerList()
{
	if(AutoLogin)
	{
		CIntroTask::instance().doConnection(CLoginClientMtp::shardIdToIndex(AutoServerId));
	}

	if(ServerListBackButton->pressed())
	{
		CIntroTask::instance().displayLoginFrame();
	}
}

void loadFrameServerList()
{
	CGuiFrame *frame = new CGuiFrame;
	frame->setUpdateCallback(updateFrameServerList);
	CGuiObjectManager::instance().registerFrame("server_list", frame);

	ServerListBackButton = new CGuiButton;
	ServerListBackButton->element(new CGuiText(ucstring("GuiBack")));
	ServerListBackButton->minWidth(80);

	ServerListView = new CGuiListView;

	guiSPG<CGuiVBox> serverListVBox = new CGuiVBox;
	frame->element(serverListVBox);

	ServerListView->rows.clear();
	serverListVBox->elements.push_back(ServerListView);
	ServerListView->eventBehaviour = new CGuiLaunchServerEventBehaviour;

	guiSPG<CGuiHBox> header = new CGuiHBox;
	header->elements.push_back(new CGuiText(ucstring("GuiServer")));
	header->elements.push_back(new CGuiText(ucstring("GuiNbPlayers")));
	ServerListView->rows.push_back(header);

	for(uint i = 0; i < CLoginClientMtp::ShardList.size(); i++)
	{
		guiSPG<CGuiHBox> row = new CGuiHBox;
		row->elements.push_back(new CGuiText(CLoginClientMtp::ShardList[i].ShardName, false));
		row->elements.push_back(new CGuiText(toString(CLoginClientMtp::ShardList[i].ShardNbPlayers), false));
		ServerListView->rows.push_back(row);
	}

	guiSPG<CGuiButton> serverListConnectButton	= new CGuiButton();
	serverListConnectButton->element(new CGuiText(ucstring("GuiConnect")));
	serverListConnectButton->minWidth(80);
	serverListConnectButton->eventBehaviour = new CGuiServerListConnectButtonEventBehaviour(ServerListView);
	guiSPG<CGuiHBox> serverListButtonBox = new CGuiHBox;
	serverListButtonBox->elements.push_back(serverListConnectButton);
	serverListButtonBox->elements.push_back(ServerListBackButton);

	CGuiSpacer *sp = new CGuiSpacer;
	sp->minHeight(10);
	serverListVBox->elements.push_back(sp);

	serverListVBox->elements.push_back(serverListButtonBox);
}
