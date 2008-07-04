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

#ifndef MT_GATE_H
#define MT_GATE_H


//
// Includes
//

#include <deque>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>

#include "interpolator.h"
#include "level_manager.h"
#include "sound_manager.h"
#include "../../common/constant.h"
#include "../../common/module_common.h"


//
// Namespaces
//

using NLMISC::CVector;
using NLMISC::CMatrix;


//
// Classes
//

class CGate : public CEditableElementCommon
{
public:
	CGate();
	virtual ~CGate();

	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot);
	
	virtual NLMISC::CVector position() { return CEditableElementCommon::position(); }
	virtual void setPosition(const NLMISC::CVector &pos);

	virtual NLMISC::CVector scale () { return CEditableElementCommon::scale(); }
	virtual void setScale (const NLMISC::CVector &scale);

private:

//////////////////////////////////////////////////////////////////////////

	LUA_BEGIN(CGate)
		LUA_BIND(CGate, getUserData),
		LUA_BIND(CGate, setUserData),
		LUA_BIND(CGate, position), LUA_BIND(CGate, setPosition),
		LUA_BIND(CGate, score), LUA_BIND(CGate, setScore),
		LUA_BIND(CGate, scale), LUA_BIND(CGate, setScale),
		LUA_BIND(CGate, module), LUA_BIND(CGate, setModule),
	LUA_END

	LUA_ACCESSOR(CLuaVector, position, Position);
	LUA_ACCESSOR(CLuaVector, scale, Scale);

	NUMBER_ACCESSOR(sint32, score, Score);

	int setMetatable(lua_State *L);
	int getUserData(lua_State *L);
	int setUserData(lua_State *L);

	int module(lua_State *L) { return 0; }
	int setModule(lua_State *L) { return 0; }

private:

	void			*LuaUserData;
	int				 LuaUserDataRef;	
};

#endif
