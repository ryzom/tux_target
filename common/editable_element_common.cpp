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

#include <nel/misc/path.h>
#include <nel/misc/triangle.h>
#include <nel/misc/plane.h>

#include "editable_element_common.h"
#include "lua_utility.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//

CEditableElementCommon::CEditableElementCommon()
{
	Type = Unknown;
	Scale = CVector(1.0f, 1.0f, 1.0f);
	Position = CVector::Null;
	Id = 255;
#ifndef MT_SERVER
	Mesh = 0;
#endif
}

CEditableElementCommon::~CEditableElementCommon()
{
}

bool CEditableElementCommon::isKindOf(TType type)
{
	return Type == type;
}

/*void CEditableElementCommon::init(const string &name, const string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation)
{
	setName(name);
	Position = position;
	Rotation = rotation;
	Scale = scale;
	Id = id;
	//_changed = false;

	NbFaces = 0;
	Normals.clear();
	Vertices.clear();
	Indices.clear();
}*/

bool CEditableElementCommon::intersect(const NLMISC::CVector &rayStart, const NLMISC::CVector &rayEnd, NLMISC::CVector &rayHit, const NLMISC::CMatrix &mat)
{
	//return true;
	//CMatrix mat = mesh()->getMatrix();
	CMatrix imat = mat;
	imat.invert();
	//rayEnd = imat * rayEnd;
	//rayStart = imat * rayStart;

	uint32 i;
	for(i = 0; i<NbFaces; i++)
	{
		CTriangle tri;
		tri.V0 = mat * Vertices[Indices[i*3+0]];
		tri.V1 = mat * Vertices[Indices[i*3+1]];
		tri.V2 = mat * Vertices[Indices[i*3+2]];
		
		CPlane p;
		p.make(tri.V0,tri.V1,tri.V2);
		CVector hit;
		bool res = tri.intersect(rayStart,rayEnd,hit,p);
		if(res)
		{
			rayHit = hit;
			return true;
		}
	}

	return false;
}

CMatrix CEditableElementCommon::transformMatrix()
{
	CMatrix res;
	res.identity();
	res.setPos(Position);
	res.setRot(Rotation);
	
	return res;
}

#ifndef MT_SERVER

using namespace NL3D;

void CEditableElementCommon::show()
{
	if(!Mesh.empty()) Mesh.show();
}

void CEditableElementCommon::hide()
{
	if(!Mesh.empty()) Mesh.hide();
}

UInstance CEditableElementCommon::mesh()
{
	return Mesh;
}

void CEditableElementCommon::renderSelection()
{
}

#endif
