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

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

#include <nel/misc/file.h>
#include <nel/misc/path.h>

#include <nel/3d/u_instance.h>

#include "level.h"
#include "3d_task.h"
#include "sky_task.h"
#include "hud_task.h"
#include "mtp_target.h"
#include "water_task.h"
#include "editor_task.h"
#include "task_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "lens_flare_task.h"
#include "entity_lua_proxy.h"
#include "module_lua_proxy.h"
#include "particles_lua_proxy.h"
#include "config_file_task.h"
#include "resource_manager2.h"
#include "../../common/lua_nel.h"
#include "../../common/lua_utility.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Types
//

//
// Declarations
//

//
// Variables
//


//
// Functions
//

CLevel::CLevel(const string &filename)
{
	nlinfo("Trying to load level '%s'", filename.c_str());

	Valid = false;
	_changed = false;

	DisplayStartPositions = CConfigFileTask::getInstance().configFile().getVar("DisplayStartPositions").asInt() == 1;
	DisplayLevel  = CConfigFileTask::getInstance().configFile().getVar("DisplayLevel").asInt() == 1;

	LuaState = luaOpenAndLoad(filename);

	if(!LuaState)
	{
		nlwarning("LEVEL: luaOpenAndLoad() failed while trying to load level '%s'", filename.c_str());
		CMtpTarget::getInstance().error(string("Missing file : "+filename));
		return;
	}

	luaGetGlobalVariable(LuaState, Name);
	nlinfo("level name '%s'", Name.c_str());
	
	luaGetGlobalVariable(LuaState, Author);
	nlinfo("author name '%s'", Author.c_str());
	
	Cameras.clear();
	luaGetGlobalVector(LuaState, Cameras);

	vector<CLuaVector> luaStartPoints;
	luaGetGlobalVectorWithName(LuaState, luaStartPoints, "StartPoints");
	uint8 startPositionId = 0;
	for(uint i = 0; i < luaStartPoints.size(); i++)
	{
		nlinfo("%g %g %g", luaStartPoints[i].x, luaStartPoints[i].y, luaStartPoints[i].z);
		CAngleAxis Rotation(CVector(1,0,0),0);
		CStartPoint *startPoint = new CStartPoint();
		startPoint->init("start pos","box.shape",startPositionId,luaStartPoints[i],Rotation);
		if (!DisplayStartPositions)
			startPoint->hide();
		StartPoints.push_back(startPoint);
		
			/*
		string res = CResourceManager::getInstance().get("col_box.shape");
		UInstance *inst = C3DTask::getInstance().scene().createInstance (res);
		if (!inst)
		{
			nlwarning ("Can't load 'col_box.shape'");
			return;
		}

		inst->setTransformMode(UTransformable::RotQuat);
		inst->setPos(StartPoints[i]);

		if (!DisplayStartPositions)
			inst->hide();

		StartPositions.push_back(inst);
		*/
		startPositionId++;
	}

	if(Cameras.size()>0)
	{
		CLuaVector defaultCam = Cameras[0];
		for(int i=Cameras.size();i<startPositionId;i++)
			Cameras.push_back(defaultCam);
	}
	
	for(uint i = 0; i < Cameras.size(); i++)
	{
		nlinfo("camera %g %g %g", Cameras[i].x, Cameras[i].y, Cameras[i].z);
		
		uint8 eid = CMtpTarget::getInstance().controler().getControledEntity();
		CEntityManager::getInstance()[eid].startPointId(CEntityManager::getInstance()[eid].rank());
		if (i == 0 || eid != 255 && CEntityManager::getInstance()[eid].startPointId() == (uint8)i)
		{
			CMtpTarget::getInstance().controler().Camera.setInitialPosition(Cameras[i]);
		}
	}
	
	

	// Load particles
	lua_getglobal(LuaState, "Particles");
	if (!lua_isnil(LuaState, -1))
	{
		lua_pushnil(LuaState);
		uint8 particlesId = 0;
		while(lua_next(LuaState, -2) != 0)
		{
			// `key' is at index -2 and `value' at index -1
			CLuaVector Position;
			lua_pushstring(LuaState,"Position");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, Position);		
			nlinfo("pos %g %g %g", Position.x, Position.y, Position.z);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			// Get module Scale
			CLuaVector Scale;
			lua_pushstring(LuaState,"Scale");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, Scale);		
			nlinfo("scale %g %g %g", Scale.x, Scale.y, Scale.z);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			// Get module rotation
			CLuaAngleAxis Rotation;
			lua_pushstring(LuaState,"Rotation");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, Rotation);		
			nlinfo("rot %g %g %g %g", Rotation.Axis.x , Rotation.Axis.y, Rotation.Axis.z, Rotation.Angle);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			uint32 Show;
			lua_pushstring(LuaState,"Show");
			lua_gettable(LuaState, -2);
			Show = (uint32)lua_tonumber(LuaState, -1);
			nlinfo("Show = %d",Show);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			uint32 Started;
			lua_pushstring(LuaState,"Started");
			lua_gettable(LuaState, -2);
			Started = (uint32)lua_tonumber(LuaState, -1);
			nlinfo("Started = %d",Started);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			// Get particles Name
			string Name;
			lua_pushstring(LuaState,"Name");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, Name);		
			nlinfo("Name  %s", Name.c_str());
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			// Get particles FileName
			string FileName;
			lua_pushstring(LuaState,"FileName");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, FileName);		
			nlinfo("FileName  %s", FileName.c_str());
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration

			CParticles *particles = new CParticles();
			particles->init(Name,FileName,particlesId,Position,Scale,Rotation,Show!=0,Started!=0);
			particlesId++;
			if(!DisplayLevel)
				particles->hide();
			
			Particles.push_back(particles);
			lua_pop(LuaState, 1);
			
		}		
	}

	// Load modules
	lua_getglobal(LuaState, "Modules");
	lua_pushnil(LuaState);
	uint8 moduleId = 0;
	while(lua_next(LuaState, -2) != 0)
	{
		// `key' is at index -2 and `value' at index -1

		// Get module position
		CLuaVector Position;
		lua_pushstring(LuaState,"Position");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Position);		
		nlinfo("pos %g %g %g", Position.x, Position.y, Position.z);
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		// Get module Scale
		CLuaVector Scale;
		lua_pushstring(LuaState,"Scale");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Scale);		
		nlinfo("scale %g %g %g", Scale.x, Scale.y, Scale.z);
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		// Get module rotation
		CLuaAngleAxis Rotation;
		lua_pushstring(LuaState,"Rotation");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Rotation);		
		nlinfo("rot %g %g %g %g", Rotation.Axis.x , Rotation.Axis.y, Rotation.Axis.z, Rotation.Angle);
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		// Get module color
		CLuaRGBA Color(255,255,255,255);
		lua_pushstring(LuaState,"Color");
		lua_gettable(LuaState, -2);
		if (!lua_isnil(LuaState, -1))
			luaGetVariable(LuaState, Color);		
		nlinfo("color = %d %d %d %d", Color.R, Color.G, Color.B, Color.A);
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		// Get module name
		string Lua;
		lua_pushstring(LuaState,"Lua");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Lua);		
		nlinfo("lua  %s", Lua.c_str());
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration

		// Get module name
		string Shape;
		lua_pushstring(LuaState,"Shape");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Shape);		
		nlinfo("Shape %s", Shape.c_str());
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		CModule *module = new CModule();
		module->init(Lua,Shape,moduleId,Position,Scale,Rotation,Color);
		moduleId++;
		if(!DisplayLevel)
			module->hide();
	
		Modules.push_back(module);
		lua_pop(LuaState, 1);
	}
	lua_pop(LuaState, 1);  // removes `key'

	// Load modules
	lua_getglobal(LuaState, "ExternalCameras");
	if (!lua_isnil(LuaState, -1))
	{
		lua_pushnil(LuaState);
		while(lua_next(LuaState, -2) != 0)
		{
			// `key' is at index -2 and `value' at index -1
			
			// Get camera position
			CLuaVector Position;
			lua_pushstring(LuaState,"Position");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, Position);		
			nlinfo("ExternalCameras pos %g %g %g", Position.x, Position.y, Position.z);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			// Get camera rotation
			CLuaAngleAxis Rotation;
			lua_pushstring(LuaState,"Rotation");
			lua_gettable(LuaState, -2);
			luaGetVariable(LuaState, Rotation);		
			nlinfo("ExternalCameras rot %g %g %g %g", Rotation.Axis.x , Rotation.Axis.y, Rotation.Axis.z, Rotation.Angle);
			lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
			
			CVector v(Position.x, Position.y, Position.z);
			CQuat q(Rotation.Axis.x, Rotation.Axis.y, Rotation.Axis.z, Rotation.Angle);
			ExternalCameras.push_back(make_pair(v, q));
			ControlerFreeLookPos = v;
			ControlerFreeLookRot = q;

			lua_pop(LuaState, 1);
		}
		lua_pop(LuaState, 1);  // removes `key'
	}

	FileName = filename;

	DisplayStartPositions = false;
	DisplayLevel = true;

	Valid = true;
	if(!C3DTask::getInstance().levelParticle().empty())
		C3DTask::getInstance().levelParticle().show();

	float cameraMinDistFromStartPointToMove = 0.5f;
	luaGetGlobalVariable(LuaState, cameraMinDistFromStartPointToMove);
	CMtpTarget::getInstance().controler().Camera.minDistFromStartPointToMove(cameraMinDistFromStartPointToMove);
	
	float cameraMinDistFromStartPointToMoveVerticaly = 0.01f;
	luaGetGlobalVariable(LuaState, cameraMinDistFromStartPointToMoveVerticaly);
	CMtpTarget::getInstance().controler().Camera.minDistFromStartPointToMoveVerticaly(cameraMinDistFromStartPointToMoveVerticaly);
	
	CLuaRGBA sunAmbientColor(82, 100, 133, 255);
	luaGetGlobalVariable(LuaState, sunAmbientColor);
	nlinfo("sunAmbientColor = %d %d %d %d", sunAmbientColor.R, sunAmbientColor.G, sunAmbientColor.B, sunAmbientColor.A);
	C3DTask::getInstance().driver().setAmbientColor(sunAmbientColor);
	C3DTask::getInstance().scene().setSunAmbient(sunAmbientColor);
	
	CLuaRGBA sunDiffuseColor(255,255,255,255);
	luaGetGlobalVariable(LuaState, sunDiffuseColor);
	C3DTask::getInstance().scene().setSunDiffuse(sunDiffuseColor);
	
	CLuaRGBA sunSpecularColor(255,255,255,255);
	luaGetGlobalVariable(LuaState, sunSpecularColor);
	C3DTask::getInstance().scene().setSunDiffuse(sunSpecularColor);
	
	CLuaVector sunDirection(-1,0,-1);
	luaGetGlobalVariable(LuaState, sunDirection);
	C3DTask::getInstance().scene().setSunDirection(sunDirection);
	
	CLuaRGBA clearColor;
	CConfigFile::CVar v;
	v = CConfigFileTask::getInstance().configFile().getVar("ClearColor");
	nlassert(v.size()==4);
	clearColor.set(v.asInt(0),v.asInt(1),v.asInt(2),v.asInt(3));	
	luaGetGlobalVariable(LuaState, clearColor);
	C3DTask::getInstance().clearColor(clearColor);
	
	float fogDistMin = CConfigFileTask::getInstance().configFile().getVar("FogDistMin").asFloat();
	luaGetGlobalVariable(LuaState, fogDistMin);
	float fogDistMax = CConfigFileTask::getInstance().configFile().getVar("FogDistMax").asFloat();
	luaGetGlobalVariable(LuaState, fogDistMax);
	CLuaRGBA fogColor = clearColor;
	luaGetGlobalVariable(LuaState, fogColor);
	C3DTask::getInstance().driver().setupFog(fogDistMin,fogDistMax,fogColor);
	
	string skyShapeFileName;
	luaGetGlobalVariable(LuaState, skyShapeFileName);
	nlinfo("skyShapeFileName '%s'", skyShapeFileName.c_str());
	CSkyTask::getInstance().shapeName(skyShapeFileName);
	
	CTaskManager::getInstance().add(CSkyTask::getInstance(), 100);
	


	string skyEnvMap0Name;
	luaGetGlobalVariable(LuaState, skyEnvMap0Name);
	nlinfo("skyEnvMap0Name '%s'", skyEnvMap0Name.c_str());
	CWaterTask::getInstance().envMap0Name(skyEnvMap0Name);
	string skyEnvMap1Name;
	luaGetGlobalVariable(LuaState, skyEnvMap1Name);
	nlinfo("skyEnvMap1Name '%s'", skyEnvMap1Name.c_str());
	CWaterTask::getInstance().envMap1Name(skyEnvMap1Name);
	string skyHeightMap0Name;
	luaGetGlobalVariable(LuaState, skyHeightMap0Name);
	nlinfo("skyHeightMap0Name '%s'", skyHeightMap0Name.c_str());
	CWaterTask::getInstance().heightMap0Name(skyHeightMap0Name);
	string skyHeightMap1Name;
	luaGetGlobalVariable(LuaState, skyHeightMap1Name);
	nlinfo("skyHeightMap1Name '%s'", skyHeightMap1Name.c_str());
	CWaterTask::getInstance().heightMap1Name(skyHeightMap1Name);
	CTaskManager::getInstance().add(CWaterTask::getInstance(), 111);
	

	CTaskManager::getInstance().add(CLensFlareTask::getInstance(), 140);

	
}


CLevel::~CLevel()
{
	nlinfo("delete level");
	CTaskManager::getInstance().remove(CWaterTask::getInstance());
	CTaskManager::getInstance().remove(CSkyTask::getInstance());
	CTaskManager::getInstance().remove(CLensFlareTask::getInstance());

	/*
	if(C3DTask::getInstance().levelParticle()!=0)
		C3DTask::getInstance().levelParticle()->hide();
		*/

	if(changed())
		CResourceManagerLan::getInstance().refresh(FileName);

	CEditorTask::getInstance().reset();

	if(LuaState)
		luaClose(LuaState);

	// removing all stuffs
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
		//C3DTask::getInstance().scene().deleteInstance(StartPositions[j]);
	}
	StartPoints.clear();
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
	nlinfo("--level reset--");
	luaClose(LuaState);

	LuaState = luaOpenAndLoad(FileName);
	if(!LuaState)
	{
		nlwarning("LEVEL: luaOpenAndLoad() failed while trying to load level '%s'", FileName.c_str());
		CMtpTarget::getInstance().error(string("Missing file : "+FileName));
		return;
	}
	
	
	luaGetGlobalVariable(LuaState, Name);
	nlinfo("level name '%s'", Name.c_str());
	
	luaGetGlobalVariable(LuaState, Author);
	nlinfo("author name '%s'", Author.c_str());
	
	Cameras.clear();
	luaGetGlobalVector(LuaState, Cameras);
	
	for(uint i = 0; i < Cameras.size(); i++)
	{
		nlinfo("camera %g %g %g", Cameras[i].x, Cameras[i].y, Cameras[i].z);
		uint8 eid = CMtpTarget::getInstance().controler().getControledEntity();
		if(ReplayFile.empty())
			CEntityManager::getInstance()[eid].startPointId(CEntityManager::getInstance()[eid].rank());
		if (i == 0 || eid != 255 && CEntityManager::getInstance()[eid].startPointId() == (uint8)i)
		{
			CMtpTarget::getInstance().controler().Camera.setInitialPosition(Cameras[i]);
		}
	}
	
/*


	vector<CLuaVector> StartPoints;
	luaGetGlobalVector(LuaState, StartPoints);
	for(uint i = 0; i < StartPoints.size(); i++)
	{
		nlinfo("%g %g %g", StartPoints[i].x, StartPoints[i].y, StartPoints[i].z);
		string res = CResourceManager::getInstance().get("col_box.shape");
		UInstance *inst = StartPositions[i];
		
		inst->setTransformMode(UTransformable::RotQuat);
		inst->setPos(StartPoints[i]);
		
		if (!DisplayStartPositions)
			inst->hide();
		
		StartPositions.push_back(inst);
	}
	
	
	// Load modules
	lua_getglobal(LuaState, "Modules");
	lua_pushnil(LuaState);
	int j = 0;
	while(lua_next(LuaState, -2) != 0)
	{
		// `key' is at index -2 and `value' at index -1
		
		// Get module position
		CLuaVector Position;
		lua_pushstring(LuaState,"Position");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Position);		
		nlinfo("pos %g %g %g", Position.x, Position.y, Position.z);
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		// Get module name
		string Name;
		lua_pushstring(LuaState,"Name");
		lua_gettable(LuaState, -2);
		luaGetVariable(LuaState, Name);		
		nlinfo("name %s", Name.c_str());
		lua_pop(LuaState, 1);  // removes `value'; keeps `key' for next iteration
		
		string res = CResourceManager::getInstance().get(Name+".shape");
		UInstance *inst = Meshes[j++];
		if (inst == 0)
		{
			nlwarning ("Can't load '%s.shape'", Name.c_str());
		}
		else
		{
			inst->setTransformMode(UTransformable::RotQuat);
			inst->setPos(Position);
			
			if (!DisplayLevel)
				inst->hide();
		}
	}
	lua_pop(LuaState, 1);  // removes `key'
	*/
	Lunar<CEntityProxy>::Register(LuaState);
	Lunar<CModuleProxy>::Register(LuaState);
	Lunar<CParticlesProxy>::Register(LuaState);
	lua_register(LuaState, "getEntityByName", getEntityByName);	
	lua_register(LuaState, "getModuleByName", getModuleByName);	
	lua_register(LuaState, "getParticlesByName", getParticlesByName);	
	lua_register(LuaState, "getEntityById", getEntityById);	
	lua_register(LuaState, "getModuleById", getModuleById);	
	lua_register(LuaState, "getParticlesById", getParticlesById);	
	CEntityManager::getInstance().luaInit();
	for(uint j = 0; j<getModuleCount();j++)
		if(Modules[j])
			Modules[j]->luaInit();
	for(uint k = 0; k<getParticlesCount();k++)
		if(Particles[k])
			Particles[k]->luaInit();
}


void CLevel::display(CLog *log) const
{
	log->displayNL("LevelName = %s", name().c_str());
	log->displayNL("There's %d starting points", StartPoints.size());
	log->displayNL("There's %d Modules", Modules.size());
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
	}
	else
	{
		for(uint j = 0; j < StartPoints.size(); j++)
		{
			StartPoints[j]->hide();
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


CModule *CLevel::getModule(uint32 index)
{
	nlassert(index<getModuleCount());
	return Modules[index];
}

CModule *CLevel::getModule(std::string &name)
{
	for(uint i = 0; i<getModuleCount();i++)
		if(Modules[i]->name()==name)
			return Modules[i];
	return 0;
}

CParticles *CLevel::getParticles(uint32 id)
{
	nlassert(id<getParticlesCount());
	return Particles[id];
	
}

CParticles *CLevel::getParticles(std::string &name)
{
	for(uint i = 0; i<getParticlesCount();i++)
		if(Particles[i]->name()==name)
			return Particles[i];
		return 0;		
}


uint32 CLevel::getParticlesCount()
{
	return Particles.size();
}

uint32 CLevel::getModuleCount()
{
	return Modules.size();
}

void CLevel::updateModule(uint32 id,const CVector &pos,const CVector &rot,uint32 selectedBy)
{
	//TODO selectedBy
	getModule(id)->update(pos,rot);//,selectedBy);
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


bool CLevel::execLuaCode(std::string code)
{
	if(LuaState)
	{
		int res = lua_dostring(LuaState,code.c_str());
		if(res==0)
			return true;
	}
	return false;
}


/////////////////////////////////////
//static
/////////////////////////////////////

int CLevel::getEntityByName(lua_State *L)
{
	size_t len;
	const char *entityName = luaL_checklstring(L, 1, &len);
	string name(entityName);
	CEntity *e = CEntityManager::getInstance().getByName(name);
	if(e)
		nlinfo("Lua(0x%p) : getEntityByName(%s))=0x%p(0x%p)",L,name.c_str(),e,e->LuaProxy);
	if(e==0)
	{
		nlwarning("getEntityByName(%s)==0",name.c_str());
		return 0;
	}
	else
		Lunar<CEntityProxy>::push(L, e->LuaProxy);
	return 1;
}


int CLevel::getModuleByName(lua_State *L)
{
	size_t len;
	const char *moduleName = luaL_checklstring(L, 1, &len);
	string name(moduleName);
	CModule *m = 0;
	if(CLevelManager::getInstance().levelPresent())
		m = CLevelManager::getInstance().currentLevel().getModule(name);
	if(m)
		nlinfo("Lua(0x%p) : getModuleById(%s))=0x%p(0x%p)",L,name.c_str(),m,m->LuaProxy);
	if(m==0)
	{
		nlwarning("getModuleById(%s)==0",name.c_str());
		return 0;
	}
	Lunar<CModuleProxy>::push(L, m->LuaProxy);
	return 1;
}


int CLevel::getParticlesByName(lua_State *L)
{
	size_t len;
	const char *particlesName = luaL_checklstring(L, 1, &len);
	string name(particlesName);
	CParticles *p = 0;
	if(CLevelManager::getInstance().levelPresent())
		p = CLevelManager::getInstance().currentLevel().getParticles(name);
	if(p)
		nlinfo("Lua(0x%p) : getParticlesByName(%s))=0x%p(0x%p)",L,name.c_str(),p,p->LuaProxy);
	if(p==0)
	{
		nlwarning("getParticlesByName(%s)==0",name.c_str());
		return 0;
	}
	Lunar<CParticlesProxy>::push(L, p->LuaProxy);
	return 1;
}


int CLevel::getEntityById(lua_State *L)
{
	uint8 id = (uint8 )luaL_checknumber(L,1);
	CEntity *e = CEntityManager::getInstance().getById(id);
	if(e)
		nlinfo("Lua(0x%p) : getEntityById(%d))=0x%p(0x%p)",L,id,e,e->LuaProxy);
	if(e==0)
	{
		nlwarning("getEntityById(%d)==0",id);
		return 0;
	}
	else
		Lunar<CEntityProxy>::push(L, e->LuaProxy);
	return 1;
}


int CLevel::getModuleById(lua_State *L)
{
	uint8 id = (uint8 )luaL_checknumber(L,1);
	CModule *m = 0;
	if(CLevelManager::getInstance().levelPresent())
		m = CLevelManager::getInstance().currentLevel().getModule(id);
	if(m)
		nlinfo("Lua(0x%p) : getModuleById(%d))=0x%p(0x%p)",L,id,m,m->LuaProxy);
	if(m==0)
	{
		nlwarning("getModuleById(%d)==0",id);
		return 0;
	}
	Lunar<CModuleProxy>::push(L, m->LuaProxy);
	return 1;
}


int CLevel::getParticlesById(lua_State *L)
{
	uint8 id = (uint8 )luaL_checknumber(L,1);
	CParticles *p = 0;
	if(CLevelManager::getInstance().levelPresent())
		p = CLevelManager::getInstance().currentLevel().getParticles(id);
	if(p)
		nlinfo("Lua(0x%p) : getParticlesById(%d))=0x%p(0x%p)",L,id,p,p->LuaProxy);
	if(p==0)
	{
		nlwarning("getParticlesById(%d)==0",id);
		return 0;
	}
	Lunar<CParticlesProxy>::push(L, p->LuaProxy);
	return 1;
}


