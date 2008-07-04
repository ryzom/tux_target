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

#include <nel/misc/file.h>
#include <nel/misc/path.h>

#include <nel/3d/u_instance.h>

#include "gate.h"
#include "level.h"
#include "3d_task.h"
#include "sky_task.h"
#include "hud_task.h"
#include "chat_task.h"
#include "particles.h"
#include "mtp_target.h"
#include "water_task.h"
#include "editor_task.h"
#include "task_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "lens_flare_task.h"
#include "config_file_task.h"
#include "../../common/lua_nel.h"
#include "../../common/lua_utility.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CLevel::CLevel(const string &filename)
{
	FileName = filename;

	Valid = false;
	DisplayStartPositions = false;
	DisplayLevel = true;

	HasBonusTime = false;
	CrashedClientCollide = true;
	AdvancedLevel = false;
	Timeout = 0.0f;
	ClientBounceCoef = 0.0f;
	ClientBounceVel= 0.0f;

	setCameraMinDistFromStartPointToMove(0.5f);
	setCameraMinDistFromStartPointToMoveVerticaly(0.01f);

	CTaskManager::instance().add(CSkyTask::instance(), 28);
	CTaskManager::instance().add(CWaterTask::instance(), 29);

	loadLevel();
}

CLevel::~CLevel()
{
	nlinfo("LEVEL: delete level");
	CTaskManager::instance().remove(CWaterTask::instance());
	CTaskManager::instance().remove(CSkyTask::instance());

	// doesn't work due to lack of zbuffer access
	//CTaskManager::instance().remove(CLensFlareTask::instance());

	CEditorTask::instance().reset();

	deleteLevel();
}

void CLevel::loadLevel()
{
	deleteLevel();

	nlinfo("LEVEL: Trying to load level '%s'", FileName.c_str());

	Valid = false;

	DisplayStartPositions = CConfigFileTask::instance().configFile().getVar("DisplayStartPositions").asInt() == 1;
	DisplayLevel  = CConfigFileTask::instance().configFile().getVar("DisplayLevel").asInt() == 1;

	CLua::instance().init();

	lua_State *L = CLua::instance().l();

	Lunar<CGate>::registerMethods(L);
	Lunar<CEntity>::registerMethods(L);
	Lunar<CModule>::registerMethods(L);
	Lunar<CParticles>::registerMethods(L);
	Lunar<CLevel>::registerMethods(L);
	lua_register(L, "entityByName", entityByName);	
	lua_register(L, "moduleByName", moduleByName);	
	lua_register(L, "particlesByName", particlesByName);	
	lua_register(L, "entityById", entityById);	
	lua_register(L, "moduleById", moduleById);	
	lua_register(L, "particlesById", particlesById);	

	CLua::instance().execute(FileName);

	CLua::instance().checkBefore();

	CLuaRGBA SunAmbientColor(82, 100, 133);
	C3DTask::instance().driver().setAmbientColor(SunAmbientColor);
	C3DTask::instance().scene().setSunAmbient(SunAmbientColor);
	C3DTask::instance().scene().setSunDiffuse(CRGBA::White);
	C3DTask::instance().scene().setSunDiffuse(CRGBA::White);

	CLuaVector SunDirection(-1, 0, -1);
	C3DTask::instance().scene().setSunDirection(SunDirection);

	CLuaRGBA ClearColor;
	CConfigFile::CVar v = CConfigFileTask::instance().configFile().getVar("ClearColor");
	nlassert(v.size()==4);
	ClearColor.set(v.asInt(0), v.asInt(1), v.asInt(2), v.asInt(3));	
	if(CLua::instance().globalVariableExists("ClearColor")) luaGetGlobalVariable(L, ClearColor);
	C3DTask::instance().clearColor(ClearColor);

	float FogDistMin = CConfigFileTask::instance().configFile().getVar("FogDistMin").asFloat();
	float FogDistMax = CConfigFileTask::instance().configFile().getVar("FogDistMax").asFloat();
	C3DTask::instance().driver().setupFog(FogDistMin, FogDistMax, ClearColor);

	// doesn't work due to lack of zbuffer access
	//CTaskManager::instance().add(CLensFlareTask::instance(), 140);

	call("init");

	DisplayLevel = true;

	Valid = true;

	CLua::instance().checkAfter();
}

void CLevel::update()
{
}

CVector CLevel::startPosition(uint32 id)
{
	if(StartPoints.size()==0) return CVector::Null;
	if(id >= StartPoints.size()) id = 0;
	return StartPoints[id]->position();
}

CVector CLevel::cameraPosition(uint32 id)
{
	if(Cameras.size()==0) return CVector::Null;
	if(id >= Cameras.size()) id = 0;
	return Cameras[id];
}

uint32 CLevel::getCameraCount()
{
	return Cameras.size();
}

void CLevel::reset()
{
	nlinfo("LEVEL: --level reset--");

	CLua::instance().init();
	CLua::instance().execute(FileName);

	lua_State *L = CLua::instance().l();

	Cameras.clear();
	luaGetGlobalVector(L, Cameras);

	for(uint i = 0; i < Cameras.size(); i++)
	{
		nlinfo("LEVEL: camera %g %g %g", Cameras[i].x, Cameras[i].y, Cameras[i].z);
		uint8 eid = CMtpTarget::instance().controler().getControledEntity();
// 		if(ReplayFile.empty())
// 			CEntityManager::instance()[eid].setStartPointId(CEntityManager::instance()[eid].rank());
		if (i == 0 || eid != 255 && CEntityManager::instance()[eid].startPointId() == (uint8)i)
		{
			CMtpTarget::instance().controler().Camera.setInitialPosition(Cameras[i]);
		}
	}
}


void CLevel::display(CLog *log) const
{
	log->displayNL("LevelName = %s", name().c_str());
	log->displayNL("There's %d starting points", StartPoints.size());
	log->displayNL("There's %d Modules", Modules.size());
	log->displayNL("There's %d Gates", Gates.size());
}

void CLevel::displayStartPositions(bool b)
{
	DisplayStartPositions = b;
	
	if(DisplayStartPositions)
	{
		for(uint j = 0; j < StartPoints.size(); j++)
		{
			StartPoints[j]->show();
		}
		for(uint j = 0; j < Gates.size(); j++)
		{
			Gates[j]->show();
		}
	}
	else
	{
		for(uint j = 0; j < StartPoints.size(); j++)
		{
			StartPoints[j]->hide();
		}
		for(uint j = 0; j < Gates.size(); j++)
		{
			Gates[j]->hide();
		}
	}
}

void CLevel::displayLevel(bool b)
{
	DisplayLevel = b;
	
	if(DisplayLevel)
	{
		for(uint i = 0; i < Modules.size(); i++)
		{
			Modules[i]->mesh().show ();
		}
	}
	else
	{
		for(uint i = 0; i < Modules.size(); i++)
		{
			Modules[i]->mesh().hide ();
		}
	}
}


CModule *CLevel::module(uint32 index)
{
	nlassert(index<moduleCount());
	return Modules[index];
}

CModule *CLevel::module(string &name)
{
	for(uint i = 0; i<moduleCount();i++)
		if(Modules[i]->name()==name)
			return Modules[i];
	return 0;
}

CParticles *CLevel::particles(uint32 id)
{
	nlassert(id<particlesCount());
	return Particles[id];
	
}

CParticles *CLevel::particles(string &name)
{
	for(uint i = 0; i<particlesCount();i++)
		if(Particles[i]->name()==name)
			return Particles[i];
		return 0;		
}


uint32 CLevel::particlesCount()
{
	return Particles.size();
}

uint32 CLevel::moduleCount()
{
	return Modules.size();
}

void CLevel::updateModule(uint32 id,const CVector &pos,const CVector &rot,uint32 selectedBy)
{
	//TODO selectedBy
	module(id)->update(pos,rot);//,selectedBy);
}

CStartPoint *CLevel::getStartPoint(uint32 index)
{
	nlassert(index<getStartPointCount());
	return StartPoints[index];
}

uint32 CLevel::getStartPointCount()
{
	nlassert(StartPoints.size()<255);
	return StartPoints.size();
}

void CLevel::updateStartPoint(uint32 id,const CVector &pos,const CVector &rot,uint32 selectedBy)
{
	//TODO selectedBy
	getStartPoint(id)->update(pos,rot);//,selectedBy);
}


bool CLevel::execLua(const string &code)
{
	lua_State *L = CLua::instance().l();
	if(!L) return false;
	int res = luaDoString(L, code.c_str());
	return (res==0);
}

const ucstring &CLevel::name() const
{
	if(Name.empty())
	{
		string fn = CFile::getFilenameWithoutExtension(FileName);
		if(CI18N::hasTranslation(fn))
		{
			Name = CI18N::get(fn);
		}
		else
		{
			Name = fn;
			nlwarning("LEVEL: Missing level translation: %s\t\t\t[]", fn.c_str());
		}
	}
	return Name;
}

const ucstring &CLevel::info() const
{
	return Info;
}
void CLevel::deleteLevel()
{
	CLua::instance().release();

	// removing all stuffs
	for(uint i = 0; i < Gates.size(); i++)
	{
		delete Gates[i];
	}
	Gates.clear();

	for(uint i = 0; i < Modules.size(); i++)
	{
		delete Modules[i];
	}
	Modules.clear();

	for(uint i = 0; i < Particles.size(); i++)
	{
		delete Particles[i];
	}
	Particles.clear();

	for(uint j = 0; j < StartPoints.size(); j++)
	{
		delete StartPoints[j];
		//C3DTask::instance().scene().deleteInstance(StartPositions[j]);
	}
	StartPoints.clear();

	ExternalCameras.clear();
}


/////////////////////////////////////
//static
/////////////////////////////////////

int CLevel::entityByName(lua_State *L)
{
	size_t len;
	const char *entityName = luaL_checklstring(L, 1, &len);
	ucstring name;
	name.fromUtf8(entityName);
	CEntity *e = CEntityManager::instance().getByName(name);
	if(e)
		nlinfo("LEVEL: Lua(0x%p) : entityByName(%s))=0x%p",L,name.toUtf8().c_str(),e);
	if(e==0)
	{
		nlwarning("LEVEL: entityByName(%s)==0",name.toUtf8().c_str());
		return 0;
	}
	else
		Lunar<CEntity>::push(L, e);
	return 1;
}


int CLevel::moduleByName(lua_State *L)
{
	size_t len;
	const char *moduleName = luaL_checklstring(L, 1, &len);
	string name(moduleName);
	CModule *m = 0;
	if(CLevelManager::instance().levelPresent())
		m = CLevelManager::instance().currentLevel().module(name);
	if(m)
		nlinfo("LEVEL: Lua(0x%p) : moduleById(%s))=0x%p",L,name.c_str(),m);
	if(m==0)
	{
		nlwarning("LEVEL: moduleById(%s)==0",name.c_str());
		return 0;
	}
	Lunar<CModule>::push(L, m);
	return 1;
}


int CLevel::particlesByName(lua_State *L)
{
	size_t len;
	const char *particlesName = luaL_checklstring(L, 1, &len);
	string name(particlesName);
	CParticles *p = 0;
	if(CLevelManager::instance().levelPresent())
		p = CLevelManager::instance().currentLevel().particles(name);
	if(p)
		nlinfo("LEVEL: Lua(0x%p) : particlesByName(%s))=0x%p",L,name.c_str(),p);
	if(p==0)
	{
		nlwarning("LEVEL: particlesByName(%s)==0",name.c_str());
		return 0;
	}
	Lunar<CParticles>::push(L, p);
	return 1;
}


int CLevel::entityById(lua_State *L)
{
	uint8 id = (uint8 )luaL_checknumber(L,1);
	CEntity *e = CEntityManager::instance().getById(id);
	//if(e) nlinfo("Lua(0x%p) : entityById(%d))=0x%p",L,id,e);
	if(e==0)
	{
		nlwarning("LEVEL: entityById(%d)==0",id);
		return 0;
	}
	else
		Lunar<CEntity>::push(L, e);
	return 1;
}


int CLevel::moduleById(lua_State *L)
{
	uint8 id = (uint8 )luaL_checknumber(L,1);
	CModule *m = 0;
	if(CLevelManager::instance().levelPresent())
		m = CLevelManager::instance().currentLevel().module(id);
	if(m)
		nlinfo("LEVEL: Lua(0x%p) : moduleById(%d))=0x%p",L,id,m);
	if(m==0)
	{
		nlwarning("LEVEL: moduleById(%d)==0",id);
		return 0;
	}
	Lunar<CModule>::push(L, m);
	return 1;
}


int CLevel::particlesById(lua_State *L)
{
	uint8 id = (uint8 )luaL_checknumber(L,1);
	CParticles *p = 0;
	if(CLevelManager::instance().levelPresent())
		p = CLevelManager::instance().currentLevel().particles(id);
	if(p)
		nlinfo("LEVEL: Lua(0x%p) : particlesById(%d))=0x%p",L,id,p);
	if(p==0)
	{
		nlwarning("LEVEL: particlesById(%d)==0",id);
		return 0;
	}
	Lunar<CParticles>::push(L, p);
	return 1;
}


int CLevel::addExternalCamera(lua_State *L)
{
	CLuaVector v = *Lunar<CLuaVector>::check(L, -2);
	CLuaAngleAxis aa = *Lunar<CLuaAngleAxis>::check(L, -1);

	CQuat q(aa.Axis.x, aa.Axis.y, aa.Axis.z, aa.Angle);
	ExternalCameras.push_back(make_pair(v, q));
	if(CMtpTarget::instance().spectator())
	{
		ControlerFreeLookPos = v;
		ControlerFreeLookRot = q;
	}
	return 0;
}

int CLevel::addModule(lua_State *L)
{
	CModule *mod = new CModule();
	if(!DisplayLevel) mod->hide();
	mod->setId(Modules.size());
	Modules.push_back(mod);

	Lunar<CModule>::push(L, mod);
	return 1;
}

int CLevel::addGate(lua_State *L)
{
	CGate *g = new CGate();
	if (!DisplayStartPositions) g->hide();
	g->setId(Gates.size());
	Gates.push_back(g);

	Lunar<CGate>::push(L, g);
	return 1;
}

int CLevel::addStartPoint(lua_State *L)
{
	CLuaVector pos = *Lunar<CLuaVector>::check(L, -1);

	CStartPoint *startPosition = new CStartPoint();

	//CAngleAxis Rotation(CVector::I, 0.0f);
	//startPosition->init("startpos", "box.shape", StartPoints.size(), pos, Rotation);
	startPosition->setPosition(pos);
	if (!DisplayStartPositions) startPosition->hide();
	StartPoints.push_back(startPosition);		

	return 0;
}

int CLevel::addCamera(lua_State *L)
{
	CLuaVector pos = *Lunar<CLuaVector>::check(L, -1);

	Cameras.push_back(pos);

	nlinfo("LEVEL: addCamera direction %f %f %f", pos.x, pos.y, pos.z);

	uint8 eid = CMtpTarget::instance().controler().getControledEntity();
	int i = Cameras.size()-1;
//	uint8 r = CEntityManager::instance()[eid].rank();
//	CEntityManager::instance()[eid].setStartPointId(CEntityManager::instance()[eid].rank());
	int sp = CEntityManager::instance()[eid].startPointId();
	if (i == 0 || eid != 255 && CEntityManager::instance()[eid].startPointId() == i)
	{
		nlinfo("LEVEL: It's my camera, set the init pos");
		CMtpTarget::instance().controler().Camera.setInitialPosition(pos);
	}

	return 0;
}

int CLevel::nbCameras(lua_State *L)
{
	lua_pushnumber(L, lua_Number(Cameras.size()));
	return 1;
}

int CLevel::setWater(lua_State *L)
{
	const char *str = lua_tostring(L,1);
	CWaterTask::instance().setWater(str);
	lua_pop(L,1);
	return 0;
}

int CLevel::setSky(lua_State *L)
{
	const char *str = lua_tostring(L,1);
	CSkyTask::instance().setSky(str);
	lua_pop(L,1);
	return 0;
}

int CLevel::setFog(lua_State *L)
{
	lua_Number fogmin = lua_tonumber(L, 1);
	lua_Number fogmax = lua_tonumber(L, 2);
	CLuaRGBA col = *Lunar<CLuaRGBA>::check(L, -1);
	C3DTask::instance().clearColor(col);
	C3DTask::instance().driver().setupFog(float(fogmin), float(fogmax), col);
	lua_pop(L,1);
	return 0;
}

int CLevel::setSun(lua_State *L)
{
	CLuaRGBA amb = *Lunar<CLuaRGBA>::check(L, -4);
	CLuaRGBA diff = *Lunar<CLuaRGBA>::check(L, -3);
	CLuaRGBA spec = *Lunar<CLuaRGBA>::check(L, -2);
	CLuaVector dir = *Lunar<CLuaVector>::check(L, -1);
	C3DTask::instance().driver().setAmbientColor(amb);
	C3DTask::instance().scene().setSunAmbient(amb);
	C3DTask::instance().scene().setSunDiffuse(diff);
	C3DTask::instance().scene().setSunDiffuse(spec);
	C3DTask::instance().scene().setSunDirection(dir);
	lua_pop(L,1);
	return 0;
}

int CLevel::setInfo(lua_State *L)
{
	const char *str = lua_tostring(L,1);
	Info = CI18N::get(string(str));
	lua_pop(L,1);
	return 0;
}
