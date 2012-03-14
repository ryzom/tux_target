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
#include "resource_manager2.h"
#include "gui_custom.h"



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
	
CGuiCustom::CGuiCustom()
{
}

CGuiCustom::~CGuiCustom()
{
}

void CGuiCustom::update()
{
	std::vector<CGuiToc *>::iterator it;
	for(it=tocList.begin();it!=tocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->update();
	}
	
}

void CGuiCustom::render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize)
{
	std::vector<CGuiToc *>::iterator it;
	for(it=tocList.begin();it!=tocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->render(pos,maxSize);
	}
	
}

void CGuiCustom::onLogin(const string &name)
{
	std::vector<CGuiToc *>::iterator it;
	for(it=tocList.begin();it!=tocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->onLogin(name);
	}
}

void CGuiCustom::onLogout(const string &name)
{
	std::vector<CGuiToc *>::iterator it;
	for(it=tocList.begin();it!=tocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->onLogout(name);
	}
}


bool CGuiCustom::load(const string &path)
{
	bool res = false;
	nlinfo("searching TOC in : %s",path.c_str());

	uint i;
	vector<string> files;
	vector<string> tocFiles;
	CPath::getPathContent(path, true, false, true, files);
	for(i=0; i<files.size(); i++)
	{
		string fn = files[i];
		uint32 p = files[i].find(".toc");
		uint32 pe = files[i].size()-4;
		if(p==pe)
		{
			tocFiles.push_back(files[i]);
			nlinfo("found : %s",files[i].c_str());
			CGuiToc *toc = CGuiToc::Load(files[i]);
			tocList.push_back(toc);
			res = true;
		}
	}
	
	return res;
}

CGuiToc *CGuiCustom::getTocByLuaState(lua_State *L)
{
	std::vector<CGuiToc *>::iterator it;
	for(it=tocList.begin();it!=tocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc->LuaState=L)
			return toc;
	}
	return 0;
}

/*
void CGuiCustom::init()
{

}

void CGuiCustom::render()
{

}

void CGuiCustom::release()
{
	
}

*/