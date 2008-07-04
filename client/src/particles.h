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

#ifndef MT_PARTICLE_H
#define MT_PARTICLE_H


//
// Includes
//

#include <deque>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>

#include "level_manager.h"


//
// Includes
//

#include <deque>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>

#include "interpolator.h"
#include "sound_manager.h"
#include "../../common/particles_common.h"


//
// Namespaces
//

using NLMISC::CVector;
using NLMISC::CMatrix;


//
// Classes
//

class CParticles : public CParticlesCommon
{
public:
	CParticles();
	virtual ~CParticles();

	virtual void enabled(bool b);

	//virtual void init(const string &name, const string &fileName, uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation, bool show, bool started);
	virtual void renderSelection();
	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot);

	virtual void setPosition(const CLuaVector &pos);
	virtual const CLuaVector &position() const { return CEditableElementCommon::position(); }

	NL3D::UParticleSystemInstance &particle() { return Particle; }

	virtual const string &name() const { return CEditableElementCommon::name(); }
	virtual void setName(const string &n) { CEditableElementCommon::setName(n); }

private:

	NL3D::UParticleSystemInstance Particle;


//////////////////////////////////////////////////////////////////////////

	int setMetatable(lua_State *L);
	int getUserData(lua_State *luaSession);
	int setUserData(lua_State *luaSession);

	int name(lua_State *luaSession);
	int getPos(lua_State *luaSession);
	int setPos(lua_State *luaSession);
	int show(lua_State *luaSession);
	int hide(lua_State *luaSession);
	int start(lua_State *luaSession);
	int stop(lua_State *luaSession);

	LUA_BEGIN(CParticles)
		LUA_BIND(CParticles, setMetatable),
		LUA_BIND(CParticles, getUserData),
		LUA_BIND(CParticles, setUserData),
		LUA_BIND(CParticles, name),
		LUA_BIND(CParticles, position), LUA_BIND(CParticles, setPosition),
		LUA_BIND(CParticles, show),
		LUA_BIND(CParticles, hide),
		LUA_BIND(CParticles, start),
		LUA_BIND(CParticles, stop),
	LUA_END

	LUA_ACCESSOR(CLuaVector, position, Position);

private:

	void		*LuaUserData;
	int			 LuaUserDataRef;	
};

#endif
