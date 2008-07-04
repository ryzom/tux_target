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

#ifndef MT_MODULE_H
#define MT_MODULE_H


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

class CModule : public CModuleCommon
{
public:
	CModule();
	virtual ~CModule();
	
	virtual void setEnabled(bool b);

	//virtual void init(const string &name, const string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation, const NLMISC::CRGBA &color);
	virtual void renderSelection();
	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot);
	
	virtual NLMISC::CVector position() { return CEditableElementCommon::position(); }
	virtual void setPosition(const NLMISC::CVector &pos);

	virtual NLMISC::CVector scale () { return CEditableElementCommon::scale(); }
	virtual void setScale (const NLMISC::CVector &scale);

	virtual NLMISC::CAngleAxis rotation() { return CEditableElementCommon::rotation(); }
	virtual void setRotation(const NLMISC::CAngleAxis &rot);

	virtual const string &name() const { return CEditableElementCommon::name(); }
	virtual void setName(const string &n);

	void setTexture(uint8 stage, const string &textureName);

	const NLMISC::CRGBA	&color() const { return Color; }
	void  setColor(const NLMISC::CRGBA &col);

private:

//////////////////////////////////////////////////////////////////////////

	LUA_BEGIN(CModule)
		LUA_BIND(CModule, getUserData),
		LUA_BIND(CModule, setUserData),
		LUA_BIND(CModule, name), LUA_BIND(CModule, setName),
		LUA_BIND(CModule, setColor),
		LUA_BIND(CModule, position), LUA_BIND(CModule, setPosition),
		LUA_BIND(CModule, bounce), LUA_BIND(CModule, setBounce),
		LUA_BIND(CModule, score), LUA_BIND(CModule, setScore),
		LUA_BIND(CModule, accel), LUA_BIND(CModule, setAccel),
		LUA_BIND(CModule, friction), LUA_BIND(CModule, setFriction),
		LUA_BIND(CModule, scale), LUA_BIND(CModule, setScale),
		LUA_BIND(CModule, rotation), LUA_BIND(CModule, setRotation),
		LUA_BIND(CModule, color), LUA_BIND(CModule, setColor),
		LUA_BIND(CModule, setTexture),
	LUA_END

	LUA_STRING_ACCESSOR(name, Name);

	LUA_ACCESSOR(CLuaVector, position, Position);
	LUA_ACCESSOR(CLuaVector, scale, Scale);
	LUA_ACCESSOR(CLuaAngleAxis, rotation, Rotation);
	LUA_ACCESSOR(CLuaRGBA, color, Color);

	BOOL_ACCESSOR(bounce, Bounce);
	BOOL_ACCESSOR(collide, Collide);

	NUMBER_ACCESSOR(sint32, score, Score);
	NUMBER_ACCESSOR(float, accel, Accel);
	NUMBER_ACCESSOR(float, friction, Friction);
	NUMBER_ACCESSOR(float, bounceVel, BounceVel);
	NUMBER_ACCESSOR(float, bounceCoef, BounceCoef);

	int setTexture(lua_State *L);

	int setMetatable(lua_State *L);
	int getUserData(lua_State *luaSession);
	int setUserData(lua_State *luaSession);

private:

	void			*LuaUserData;
	int				 LuaUserDataRef;	
	NLMISC::CRGBA	 Color;
};

#endif
