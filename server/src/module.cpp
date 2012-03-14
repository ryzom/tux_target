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

#include <nel/misc/path.h>

#include "variables.h"
#include "module.h"
#include "physics.h"
#include "network.h"
#include "lua_engine.h"
#include "../../common/load_mesh.h"
#include "../../common/lua_utility.h"

//
// Namespaces
//

using namespace std;
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

static int _dTriCallback(dGeomID TriMesh, dGeomID RefObject, int TriangleIndex)
{
	return 0;
}

static void _dTriArrayCallback(dGeomID TriMesh, dGeomID RefObject, const int* TriIndices, int TriCount)
{
}

static int _dTriRayCallback(dGeomID TriMesh, dGeomID Ray, int TriangleIndex, dReal u, dReal v)
{
	return 0;
}

CModule::CModule() : CModuleCommon()
{
	luaProxy = 0;
	triMeshDataId = 0;
	BounceCoef = BounceScene;
	BounceVel = BounceVelScene;	
}

void CModule::init(const std::string &name, const std::string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation, const NLMISC::CRGBA &color)
{
	CModuleCommon::init(name, shapeName, id, position, scale, rotation,color);
	
	// Get collision faces
	loadMesh(ShapeName, Vertices, Normals, Indices, AutoEdges,true);

	CMatrix scaleMat;
	scaleMat.identity();
	scaleMat.setScale(scale);
	
	Geom = 0;
	luaProxy = 0;
	Visible = true;
	_luaInit();
	
	// vertices must be multiple of 3
	OdeVertices.resize(Vertices.size()*3);
	for(uint i = 0; i < Vertices.size(); i++)
	{
		CVector p = scaleMat * Vertices[i];
		OdeVertices[i*3+0] = p.x;
		OdeVertices[i*3+1] = p.y;
		OdeVertices[i*3+2] = p.z;
//		nlinfo("%d %f %f %f", i, Vertices[i*3+0], Vertices[i*3+1], Vertices[i*3+2]);
	}
	triMeshDataId = dGeomTriMeshDataCreate();

#ifdef _ODE_COMPATIBILITY_H_ //if this is defined we use ode 0.5
	//dGeomTriMeshDataBuildSimple(triMeshDataId, &OdeVertices[0], OdeVertices.size(), &Indices[0], Indices.size());
#if defined(dDOUBLE) //we use ode 0.5 so we could manage single or double precision
	dGeomTriMeshDataBuildDouble(triMeshDataId, &OdeVertices[0], 3*sizeof(dReal), OdeVertices.size()/3, &Indices[0], Indices.size(), 3*sizeof(int));
#else
	dGeomTriMeshDataBuildSingle(triMeshDataId, &OdeVertices[0], 3*sizeof(dReal), OdeVertices.size()/3, &Indices[0], Indices.size(), 3*sizeof(int));
#endif
#else
#if defined(dDOUBLE) 
#error // dont use ode 0.039 in double mode
#else
	dGeomTriMeshDataBuild(triMeshDataId, &OdeVertices[0], 3*sizeof(dReal), OdeVertices.size()/3, &Indices[0], Indices.size(), 3*sizeof(int));
#endif
#endif
	
	{
		CSynchronized<dSpaceID>::CAccessor acces(&Space);
		Geom = dCreateTriMesh(acces.value(), triMeshDataId, 0/*_dTriCallback*/, _dTriArrayCallback, _dTriRayCallback);
	}
	nlassert(Geom);
	dGeomSetData(Geom, this);

	//  OLD ODE CODE FOR TRI COL
	//	dGeomID userGeomId;
	//	entity->geom = userGeomId = dCreateTriList(space, _dTriCallback, _dTriArrayCallback, _dTriRayCallback);
	//	dGeomSetData(userGeomId, entity);
	//
	//	dVector3 *f = &entity->vertices[0];
	//	dGeomTriListBuild(userGeomId, &entity->vertices[0], sizeof(dcVector3), entity->vertices.size(), &entity->indices[0], sizeof(int), entity->indices.size(), 3 * sizeof(int));

	dGeomSetPosition(Geom, position.x, position.y, position.z);
	dMatrix3 R;
	dRFromAxisAndAngle(R, rotation.Axis.x, rotation.Axis.y, rotation.Axis.z, rotation.Angle);
	dGeomSetRotation(Geom, R);
	/*	entity->setRotation(CAngleAxis(CVector(triColl.axisX, triColl.axisY, triColl.axisZ), triColl.angle));
	
	entity->Score = triColl.score;
	entity->Accel = triColl.accel;
	entity->Friction = triColl.friction;
	entity->Bounce = triColl.bounce;
	entity->LuaFunctionName = triColl.luaFunctionName;
	entity->Name = triColl.name;
*/	
	dGeomSetCategoryBits(Geom, MT_SCENE_SPECTRUM);
	dGeomSetCollideBits(Geom, MT_CLIENT_SPECTRUM);

}


void CModule::_luaInit()
{
	luaProxy = new CModuleProxy(CLuaEngine::getInstance().session(),this);

	// load the lua code for this module
	string filename = CPath::lookup("module_" + Name + ".lua", false);
	if(filename.empty())
	{
		nlinfo("MODULE: There's no lua code for module '%s', using default value", filename.c_str());
		return;
	}
	

	lua_pushliteral(CLuaEngine::getInstance().session(), "module");
	Lunar<CModuleProxy>::push(CLuaEngine::getInstance().session(), luaProxy);
	lua_settable(CLuaEngine::getInstance().session(), LUA_GLOBALSINDEX);
	
	luaLoad(CLuaEngine::getInstance().session(),filename);

}

CModule::~CModule()
{
	if(luaProxy)
		delete luaProxy;
	luaProxy = 0;
	if(Geom)
	{
		if(triMeshDataId)
		{
			dGeomTriMeshDataDestroy(triMeshDataId);
			triMeshDataId = 0;
		}

		dGeomSetData(Geom, (void *)0xDEADBEEF);
		dGeomDestroy(Geom);
		Geom = 0;
	}
}

/*
NLMISC::CVector CModule::position() const
{
	return Position;

	CVector pos;
	nlassert(Geom);
	const dReal *p = dGeomGetPosition(Geom);
	if(p)
	{
		pos.x = p[0];
		pos.y = p[1];
		pos.z = p[2];
	}
	else
		pos = NLMISC::CVector::Null;

	return pos;

}

*/


void CModule::update() 
{
	if(luaProxy)
		luaProxy->call("update");
}

void CModule::initBeforeStartLevel()
{
	if(luaProxy)
		luaProxy->call("init");
}


void CModule::update(const CVector &pos, const CVector &rot)
{	
	pausePhysics();
	dGeomSetPosition(Geom, pos.x, pos.y, pos.z);
	resumePhysics();
	Position = pos;
	_changed = true;
}

void CModule::visible(bool e) 
{
	if(Visible==e) return;
	
	nlinfo("visible element %s to %s",name().c_str(),e?"true":"false");
	CNetMessage msgout(CNetMessage::EnableElement);
	msgout.serial(_id);
	msgout.serial(e);
	CNetwork::getInstance().send(msgout);
}

void CModule::enabled(bool e) 
{
	if(Enabled==e) return;

	nlinfo("enable element %s to %s",name().c_str(),e?"true":"false");
	CModuleCommon::enabled(e);
	
	CNetMessage msgout(CNetMessage::EnableElement);
	msgout.serial(_id);
	msgout.serial(e);
	CNetwork::getInstance().send(msgout);

	pausePhysics();
	if(e)
		dGeomEnable(Geom);
	else
		dGeomDisable(Geom);
	resumePhysics();
	
}

void CModule::changePosition(NLMISC::CVector &pos) 
{
	pausePhysics();
	dGeomSetPosition(Geom, pos.x, pos.y, pos.z);
	resumePhysics();
	Position = pos;
	CNetMessage msgout(CNetMessage::UpdateElement);
	uint8 elementType = CEditableElementCommon::Module;
	uint8 elementId = id();
	uint8 selectedBy = 0;
	CVector eulerRot(0,0,0);
	msgout.serial(elementType);
	msgout.serial(elementId);
	msgout.serial(selectedBy);
	msgout.serial(pos);
	msgout.serial(eulerRot);
	CNetwork::getInstance().send(msgout);
}


//
//
//
