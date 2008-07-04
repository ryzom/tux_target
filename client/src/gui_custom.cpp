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
#include "time_task.h"
#include "gui_custom.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


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
/*	vector<CGuiToc *>::iterator it;
	for(it=TocList.begin();it!=TocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->update();
	}
*/	
}

void CGuiCustom::render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize)
{
/*	vector<CGuiToc *>::iterator it;
	for(it=TocList.begin();it!=TocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->render(pos,maxSize);
	}
*/	
}

void CGuiCustom::onLogin(const ucstring &name)
{
/*	vector<CGuiToc *>::iterator it;
	for(it=TocList.begin();it!=TocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->onLogin(name);
	}*/
}

void CGuiCustom::onLogout(const ucstring &name)
{
/*	vector<CGuiToc *>::iterator it;
	for(it=TocList.begin();it!=TocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc)
			toc->onLogout(name);
	}*/
}


bool CGuiCustom::load(const string &path)
{
	bool res = false;
/*	nlinfo("searching TOC in : %s",path.c_str());

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
			TocList.push_back(toc);
			res = true;
		}
	}
*/	
	return res;
}

/*CGuiToc *CGuiCustom::getTocByLuaState(lua_State *L)
{
	vector<CGuiToc *>::iterator it;
	for(it=TocList.begin();it!=TocList.end();it++)
	{
		CGuiToc *toc = *it;
		if(toc->LuaState=L)
			return toc;
	}
	return 0;
}*/

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
