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
// Includes
//

#include "stdpch.h"

#include "3d_task.h"
#include "time_task.h"
#include "gui_task.h"
#include "resource_manager2.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	
static float percent = 0.12f;
static guiSPG<CGuiFrame> testFrame = 0;

void CGuiTask::init()
{
	CGuiObjectManager::getInstance().init();


	//test code
	/*
	guiSPG<CGuiXml> xml = 0;
	xml = CGuiXmlManager::getInstance().Load("server_list.xml");
	testFrame = (CGuiFrame *)xml->get("serverListFrame");
	CGuiObjectManager::getInstance().objects.push_back(testFrame);
	guiSPG<CGuiVBox> serverVbox = (CGuiVBox *)xml->get("serverVbox");

	xml = CGuiXmlManager::getInstance().Load("server_item.xml");
	{
		guiSPG<CGuiHBox> serverItemHBox = (CGuiHBox *)xml->get("serverItemHBox");
		guiSPG<CGuiText> btextServer = (CGuiText *)xml->get("btextServer");
		btextServer->text = "mtp-target.dyndns.org";
		serverVbox->elements.push_back(serverItemHBox);
	}
	
	xml = CGuiXmlManager::getInstance().Load("server_item.xml");
	{
		guiSPG<CGuiHBox> serverItemHBox = (CGuiHBox *)xml->get("serverItemHBox");
		guiSPG<CGuiText> btextServer = (CGuiText *)xml->get("btextServer");
		btextServer->text = "toto.dyndns.org";
		serverVbox->elements.push_back(serverItemHBox);
	}
	
	xml = CGuiXmlManager::getInstance().Load("server_item.xml");
	{
		guiSPG<CGuiHBox> serverItemHBox = (CGuiHBox *)xml->get("serverItemHBox");
		guiSPG<CGuiText> btextServer = (CGuiText *)xml->get("btextServer");
		btextServer->text = "skeet.dyndns.org";
		serverVbox->elements.push_back(serverItemHBox);
	}
	*/
	
	
	/*
	guiSPG<CGuiXml> xml = CGuiXmlManager::getInstance().Load("menu.xml");
	testFrame = (CGuiFrame *)xml->get("menuFrame");

	CGuiObjectManager::getInstance().objects.push_back(testFrame);
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
	C3DTask::getInstance().driver().setMatrixMode2D11 ();
	C3DTask::getInstance().driver().setMatrixMode2D (CFrustum(0,640,0,480,-1,1,false));
	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	CGuiObjectManager::getInstance().render();
}

void CGuiTask::release()
{
	CGuiObjectManager::getInstance().release();
}
