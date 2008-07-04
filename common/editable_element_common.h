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

#ifndef MT_EDITABLE_ELEMENT_COMMON_H
#define MT_EDITABLE_ELEMENT_COMMON_H


//
// Includes
//

#include "lua_utility.h"
#include "lua_nel.h"
#include "lunar.h"
#include "load_mesh.h"

#ifndef MT_SERVER
#include <nel/3d/u_material.h>
#endif

//
// Classes
//

class CEditableElementCommon
{
public:
	enum TType
	{
		Unknown = 0,
		Module,
		StartPosition,
		Particles,
		Gate,
	};		
	
	CEditableElementCommon();
	
	virtual ~CEditableElementCommon();

	//virtual void init(const string &name, const string &shapeName,uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation);
	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot) = 0;
	virtual bool intersect(const NLMISC::CVector &rayStart, const NLMISC::CVector &rayEnd, NLMISC::CVector &rayHit, const NLMISC::CMatrix &mat);
	
	virtual const CLuaVector &position() const { return Position; }
	virtual void setPosition(const CLuaVector &pos) { Position = pos; }

	virtual const CLuaVector &scale() const { return Scale; }
	virtual void setScale(const CLuaVector &scale) { Scale = scale; }

	virtual const CLuaAngleAxis &rotation() const { return Rotation; }
	virtual void setRotation(const CLuaAngleAxis &rot) { Rotation = rot; }

	virtual NLMISC::CMatrix transformMatrix();
	
	TType type() { return Type; }

	void setId(uint8 id) { Id = id; }
	uint8 id() const { return Id; }
	
	virtual const string &name() const { return Name; }
	virtual void setName(const string &n)
	{
		nlinfo("LEVEL: setName changes from %s to %s", Name.c_str(), n.c_str());
		Name = n;
	}

	bool isKindOf(TType type);

#ifndef MT_SERVER
	NL3D::UInstance mesh();
	virtual void renderSelection();
	void show();
	void hide();
#endif

	vector<CAutoEdge>		AutoEdges;
protected:
	vector<NLMISC::CVector> Normals;
	vector<NLMISC::CVector> Vertices;
	vector<int>				Indices;
	uint32					NbFaces;
	
	string				Name;
	string				ShapeName;
	TType				Type;
	uint8				Id;

#ifndef MT_SERVER
	NL3D::UInstance		Mesh;
	NL3D::UMaterial		mat;
#endif

private:
	CLuaVector		Position;
	CLuaVector		Scale;
	CLuaAngleAxis	Rotation;
};

#endif
