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
#include "particles.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CParticles::CParticles() : CParticlesCommon()
{
	mat = C3DTask::instance().createMaterial();
	LuaUserDataRef = 0;
	LuaUserData = 0;
}


CParticles::~CParticles()
{
	if(!Mesh.empty())
	{
		C3DTask::instance().scene().deleteInstance(Mesh);
	}
}


/*void CParticles::init(const string &name, const string &fileName, uint8 id, const CVector &position, const CVector &scale, const CAngleAxis &rotation, bool show, bool started)
{
	CParticlesCommon::init(name, fileName, id, position, scale, rotation,show,started); 

	ShapeName = CPath::lookup(fileName+".ps");

	string res = CPath::lookup(ShapeName);
	Mesh = C3DTask::instance().scene().createInstance(res);
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s'", ShapeName.c_str());
	}
	Particle.cast(Mesh);
	Particle.setTransformMode (UTransformable::RotQuat);
	Particle.setOrderingLayer(2);
	Particle.activateEmitters(started);
	if(show)
		Particle.show();
	else
		Particle.hide();
	Particle.setPos(position);
}*/

void CParticles::renderSelection()
{
	mat.setColor(CRGBA(255,255,255,200));
	mat.setZWrite(true);
	mat.setZFunc(UMaterial::always);
	mat.setBlend(true);
	mat.setBlendFunc(UMaterial::srcalpha,UMaterial::invsrcalpha);

	CMatrix matrix = mesh().getMatrix();
}

void CParticles::update(const NLMISC::CVector &pos, const NLMISC::CVector &rot)
{
	//TODO rot
	setPosition(pos);
}


void CParticles::enabled(bool b)
{
	if(Enabled==b) return;

	CParticlesCommon::enabled(b);
	Particle.activateEmitters(b);

	nlinfo("%s %s",b?"show":"hide",name().c_str());
}

void CParticles::setPosition(const CLuaVector &pos)
{
	CEditableElementCommon::setPosition(pos);
	Mesh.setPos(pos);
}

//////////////////////////////////////////////////////////////////////////


int CParticles::setMetatable(lua_State *L)
{ 
	int res = lua_setmetatable(L, -2);
	if(res==0)
		nlwarning("cannot set metatable");
	return 0; 
}


int CParticles::getUserData(lua_State *luaSession)
{
	lua_getref(luaSession, LuaUserDataRef); //push obj which have this ref id
	return 1; // one return value
}

int CParticles::setUserData(lua_State *luaSession)
{
	LuaUserData = lua_touserdata(luaSession, 1); // get arg
	LuaUserDataRef = lua_ref(luaSession,1); //get ref id and lock it
	return 0; // no return value
}

int CParticles::name(lua_State *luaSession)
{
	lua_pushstring(luaSession,name().c_str());
	return 1;
}




/*int CParticles::getPos(lua_State *luaSession)
{
	Pos = position();
	Lunar<CLuaVector>::push(luaSession,&Pos);
	return 1;
}

int CParticles::setPos(lua_State *luaSession)
{
	CLuaVector pos  = *Lunar<CLuaVector>::check(luaSession,-1);
	position(pos);
	return 0;
}*/

int CParticles::show(lua_State *luaSession)
{
	particle().show();
	return 0;
}

int CParticles::hide(lua_State *luaSession)
{
	particle().hide();
	return 0;
}

int CParticles::start(lua_State *luaSession)
{
	particle().activateEmitters(true);
	return 0;
}

int CParticles::stop(lua_State *luaSession)
{
	particle().activateEmitters(false);
	return 0;
}
