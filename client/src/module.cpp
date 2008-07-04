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

#include "module.h"
#include "3d_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CModule::CModule() : CModuleCommon()
{
	mat = C3DTask::instance().createMaterial();
	LuaUserDataRef = 0;
	LuaUserData = 0;
	Bounce = true;
	Collide = true;
	Score = 0;
	Accel = 0.0f;
	Friction = 0.0f;
	BounceVel = 0.0f;
	BounceCoef = 0.0f;
	Color = CRGBA::White;
}


CModule::~CModule()
{
	if(!Mesh.empty())
	{
		C3DTask::instance().scene().deleteInstance(Mesh);
	}
}

/*void CModule::init(const string &name, const string &shapeName, uint8 id, const CVector &position, const CVector &scale, const CAngleAxis &rotation, const NLMISC::CRGBA &color)
{
	CModuleCommon::init(name, shapeName, id, position, scale, rotation,color);

	// Get collision faces

	ShapeName = shapeName;
	NbFaces = loadMesh(ShapeName+".shape", Vertices, Normals, Indices, AutoEdges);

	Mesh = C3DTask::instance().scene().createInstance (CPath::lookup(ShapeName+".shape"));
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s.shape'", Name.c_str());
	}
	Mesh.setTransformMode(UTransformable::RotQuat);
	Mesh.setRotQuat(CQuat(rotation));
	Mesh.setPos(position);
	CVector oldScale = Mesh.getScale();
	oldScale.x *= scale.x;
	oldScale.y *= scale.y;
	oldScale.z *= scale.z;
	Mesh.setScale(oldScale);
	Color.set(255,255,255);
	this->setColor(color);

	if(Shadow)
	{
		Mesh.enableReceiveShadowMap(true);
		Mesh.setShadowMapDirectionZThreshold(-0.86f);
		Mesh.setShadowMapMaxDepth(0.5f);
		NL3D::UVisualCollisionMesh colMesh;
		Mesh.getShape().getVisualCollisionMesh(colMesh);
		// if this mesh has a collision
		if(!colMesh.empty())
		{
			// get the instance matrix
			const CMatrix &mat = Mesh.getMatrix();

			// then send the result to the collision manager, and keep the mesh col id if succeed
			uint32     meshId = C3DTask::instance().collisionManager().addMeshInstanceCollision(colMesh, mat, false, false);
			//			if(meshId)
			//				mg.Meshs.push_back(meshId);
		}
	}
}*/

void CModule::renderSelection()
{
	mat.setColor(CRGBA(255,255,255,200));
	mat.setZWrite(true);
	mat.setZFunc(UMaterial::always);
	mat.setBlend(true);
	mat.setBlendFunc(UMaterial::srcalpha,UMaterial::invsrcalpha);

	CMatrix matrix = mesh().getMatrix();

	float grow = 0.001f;
	uint i;
	for(i = 0; i<NbFaces; i++)
	{
		CTriangle tri;
		tri.V0 = matrix * (Vertices[Indices[i*3+0]] + grow * Normals[Indices[i*3+0]]);
		tri.V1 = matrix * (Vertices[Indices[i*3+1]] + grow * Normals[Indices[i*3+1]]);
		tri.V2 = matrix * (Vertices[Indices[i*3+2]] + grow * Normals[Indices[i*3+2]]);

#if 0
		C3DTask::instance().driver().drawTriangle(tri,mat);
#else
		CLine line1(tri.V0,tri.V1);
		CLine line2(tri.V1,tri.V2);
		CLine line3(tri.V2,tri.V0);

		C3DTask::instance().driver().drawLine(line1,mat);
		C3DTask::instance().driver().drawLine(line2,mat);
		C3DTask::instance().driver().drawLine(line3,mat);
#endif

	}

}

void CModule::update(const NLMISC::CVector &pos, const NLMISC::CVector &rot)
{
	//setRotation(rot);
	setPosition(pos);
}

void CModule::setEnabled(bool b)
{
	if(Enabled==b || Mesh.empty()) return;

	CModuleCommon::setEnabled(b);

	if(b)
		Mesh.show();
	else
		Mesh.hide();

	nlinfo("%s %s",b?"show":"hide",name().c_str());
}

//////////////////////////////////////////////////////////////////////////

int CModule::setMetatable(lua_State *L)
{
	int res = lua_setmetatable(L, -2);
	if(res==0) nlwarning("Cannot set metatable");
	return 0;
}


int CModule::getUserData(lua_State *luaSession)
{
	lua_getref(luaSession, LuaUserDataRef); //push obj which have this ref id
	/*
	if(!LuaUserDataRef)
	nlwarning("clien lua call getuserdata but userdata is 0");
	*/
	return 1; // one return value
}

int CModule::setUserData(lua_State *luaSession)
{
	LuaUserData = lua_touserdata(luaSession, 1); // get arg
	LuaUserDataRef = lua_ref(luaSession,1); //get ref id and lock it
	return 0; // no return value
}

void CModule::setName(const string &n)
{
	CEditableElementCommon::setName(n);

	// Get collision faces

	ShapeName = n;
	NbFaces = loadMesh(ShapeName+".shape", Vertices, Normals, Indices, AutoEdges);

	Mesh = C3DTask::instance().scene().createInstance (CPath::lookup(ShapeName+".shape"));
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s.shape'", n.c_str());
		return;
	}

	if(Shadow)
	{
		Mesh.enableReceiveShadowMap(true);
		Mesh.setShadowMapDirectionZThreshold(-0.86f);
		Mesh.setShadowMapMaxDepth(0.5f);
		NL3D::UVisualCollisionMesh colMesh;
		Mesh.getShape().getVisualCollisionMesh(colMesh);
		// if this mesh has a collision
		if(!colMesh.empty())
		{
			// get the instance matrix
			const CMatrix &mat = Mesh.getMatrix();

			// then send the result to the collision manager, and keep the mesh col id if succeed
			uint32     meshId = C3DTask::instance().collisionManager().addMeshInstanceCollision(colMesh, mat, false, false);
			//			if(meshId)
			//				mg.Meshs.push_back(meshId);
		}
	}
}

void CModule::setPosition(const NLMISC::CVector &pos)
{
	CEditableElementCommon::setPosition(pos);

	nlassert(!Mesh.empty());
	Mesh.setPos(pos);
}

void CModule::setScale(const NLMISC::CVector &scale)
{
	CEditableElementCommon::setScale(scale);

	nlassert(!Mesh.empty());
	CVector oldScale = Mesh.getScale();
	oldScale.x *= scale.x;
	oldScale.y *= scale.y;
	oldScale.z *= scale.z;
	Mesh.setScale(oldScale);
}

void CModule::setRotation(const CAngleAxis &rot)
{
	CEditableElementCommon::setRotation(rot);

	nlassert(!Mesh.empty());
	Mesh.setTransformMode(UTransformable::RotQuat);
	Mesh.setRotQuat(CQuat(rot));
}

void CModule::setColor(const CRGBA &col)
{
	nlassert(!Mesh.empty());

	nlinfo("LEVEL: setColor changes from %d %d %d %d to %d %d %d %d", Color.R, Color.G, Color.B, Color.A, col.R, col.G, col.B, col.A);

	Color = col;
	for(uint i = 0; i < Mesh.getNumMaterials(); i++)
	{
		Mesh.getMaterial(i).setDiffuse(Color);
		Mesh.getMaterial(i).setAmbient(Color);
	}
}

void CModule::setTexture(uint8 stage, const string &textureName)
{
	if(textureName.empty())
		return;

	string TextureFilename = CPath::lookup(textureName, false);
	if(TextureFilename.empty())
		return;

	// set the texture now if object is available
	if(Mesh.empty())
		return;

	for(uint i = 0; i < Mesh.getNumMaterials(); i++)
	{
		Mesh.getMaterial(i).setTextureFileName(TextureFilename, stage);
	}
}

int CModule::setTexture(lua_State *L)
{
	lua_Number stage = lua_tonumber(L,1);
	const char *str = lua_tostring(L,2);
	setTexture(uint8(stage), string(str)+".dds");
	lua_pop(L,1);
	return 0;
}
