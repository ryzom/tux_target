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

#include "3d_task.h"
#include "gui_task.h"
#include "time_task.h"


//
// Namespaces
//

using namespace NL3D;


//
// Functions
//

//static float percent = 0.12f;
//static guiSPG<CGuiFrame> testFrame = 0;

void CGuiTask::init()
{
	CGuiObjectManager::instance().init();


	//test code
	/*
	guiSPG<CGuiXml> xml = 0;
	xml = CGuiXmlManager::instance().Load("server_list.xml");
	testFrame = (CGuiFrame *)xml->get("serverListFrame");
	CGuiObjectManager::instance().objects.push_back(testFrame);
	guiSPG<CGuiVBox> serverVbox = (CGuiVBox *)xml->get("serverVbox");

	xml = CGuiXmlManager::instance().Load("server_item.xml");
	{
		guiSPG<CGuiHBox> serverItemHBox = (CGuiHBox *)xml->get("serverItemHBox");
		guiSPG<CGuiText> btextServer = (CGuiText *)xml->get("btextServer");
		btextServer->text = "mtp-target.dyndns.org";
		serverVbox->elements.push_back(serverItemHBox);
	}
	
	xml = CGuiXmlManager::instance().Load("server_item.xml");
	{
		guiSPG<CGuiHBox> serverItemHBox = (CGuiHBox *)xml->get("serverItemHBox");
		guiSPG<CGuiText> btextServer = (CGuiText *)xml->get("btextServer");
		btextServer->text = "toto.dyndns.org";
		serverVbox->elements.push_back(serverItemHBox);
	}
	
	xml = CGuiXmlManager::instance().Load("server_item.xml");
	{
		guiSPG<CGuiHBox> serverItemHBox = (CGuiHBox *)xml->get("serverItemHBox");
		guiSPG<CGuiText> btextServer = (CGuiText *)xml->get("btextServer");
		btextServer->text = "skeet.dyndns.org";
		serverVbox->elements.push_back(serverItemHBox);
	}
	*/
	
	
	/*
	guiSPG<CGuiXml> xml = CGuiXmlManager::instance().Load("menu.xml");
	testFrame = (CGuiFrame *)xml->get("menuFrame");

	CGuiObjectManager::instance().objects.push_back(testFrame);
	*/
	
	
	/*
	CGuiProgressBar *progressBar = (CGuiProgressBar *)xml->get("progressBar");
	progressBar->ptrValue(&percent);

	CGuiHScale *hscale = (CGuiHScale *)xml->get("hscale");
	hscale->ptrValue(&percent);

	CGuiTextPercent *stext = (CGuiTextPercent *)xml->get("textPercent");
	stext->ptrValue(&percent);
	*/
	
}

void CGuiTask::render()
{
	C3DTask::instance().driver().setMatrixMode2D11 ();
	C3DTask::instance().driver().setMatrixMode2D (CFrustum(0,640,0,480,-1,1,false));
	C3DTask::instance().driver().setFrustum(CFrustum(0, (float)C3DTask::instance().screenWidth(), 0, (float)C3DTask::instance().screenHeight(), -1, 1, false));
	CGuiObjectManager::instance().render();
}

void CGuiTask::update()
{
	CGuiObjectManager::instance().update();
}

void CGuiTask::release()
{
	CGuiObjectManager::instance().release();
}
