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

#include "gate.h"
#include "3d_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CGate::CGate() : CEditableElementCommon()
{
	LuaUserDataRef = 0;
	LuaUserData = 0;

	Score = 0;

	ShapeName = CPath::lookup("col_box.shape");

	Mesh = C3DTask::instance().scene().createInstance (ShapeName);
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s'", ShapeName.c_str());
	}
}

CGate::~CGate()
{
	if(!Mesh.empty())
	{
		C3DTask::instance().scene().deleteInstance(Mesh);
	}
}

void CGate::update(const NLMISC::CVector &pos, const NLMISC::CVector &rot)
{
	setPosition(pos);
}

//////////////////////////////////////////////////////////////////////////

int CGate::setMetatable(lua_State *L)
{
	int res = lua_setmetatable(L, -2);
	if(res==0)
		nlwarning("cannot set metatable");
	return 0;
}


int CGate::getUserData(lua_State *L)
{
	lua_getref(L, LuaUserDataRef); //push obj which have this ref id
	/*
	if(!LuaUserDataRef)
	nlwarning("client lua call getuserdata but userdata is 0");
	*/
	return 1; // one return value
}

int CGate::setUserData(lua_State *L)
{
	LuaUserData = lua_touserdata(L, 1); // get arg
	LuaUserDataRef = lua_ref(L,1); //get ref id and lock it
	return 0; // no return value
}

void CGate::setPosition(const NLMISC::CVector &pos)
{
	CEditableElementCommon::setPosition(pos);

	if(!Mesh.empty()) Mesh.setPos(pos);
}

void CGate::setScale(const NLMISC::CVector &scale)
{
	CEditableElementCommon::setScale(scale);

	if(!Mesh.empty())
	{
		CVector oldScale = Mesh.getScale();
		oldScale.x *= scale.x;
		oldScale.y *= scale.y;
		oldScale.z *= scale.z;
		Mesh.setScale(oldScale);
	}
}
