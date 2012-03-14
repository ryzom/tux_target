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

#ifndef MTPT_EDITABLE_ELEMENT_COMMON
#define MTPT_EDITABLE_ELEMENT_COMMON


//
// Includes
//

#include <vector>

#include "lua_utility.h"
#include "lua_nel.h"
#include "lunar.h"
#include "load_mesh.h"

#ifndef MTPT_SERVER
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
	};		
	
	CEditableElementCommon();
	
	virtual ~CEditableElementCommon();

	virtual void init(const std::string &name, const std::string &shapeName,uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation);
	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot) = 0;
	virtual std::string toLuaString() = 0;
	virtual bool intersect(const NLMISC::CVector &rayStart, const NLMISC::CVector &rayEnd, NLMISC::CVector &rayHit, const NLMISC::CMatrix &mat);
	
	virtual NLMISC::CVector position() const {return Position;}
	virtual void position(const NLMISC::CVector &pos) {Position = pos; _changed = true;}
	virtual NLMISC::CMatrix transformMatrix();
	virtual NLMISC::CAngleAxis rotation();
	
	TType type() { return _type;}
	uint8 id() {return _id;}
	const std::string	&name() const { return Name; }
	bool isKindOf(TType type);

	bool changed() {return _changed;}
	void changed(bool c) {_changed = c;}

#ifndef MTPT_SERVER
	NL3D::UInstance mesh();
	virtual void renderSelection();
	void show();
	void hide();
#endif

	std::vector<CAutoEdge> AutoEdges;
protected:
	std::vector<NLMISC::CVector> Normals;
	std::vector<NLMISC::CVector> Vertices;
	std::vector<int>			 Indices;
	uint32 NbFaces;
	
	std::string			Name;
	std::string			ShapeName;
	NLMISC::CVector		Position;
	NLMISC::CVector		Scale;
	NLMISC::CAngleAxis	Rotation;
	bool				_changed;	
	TType				_type;
	uint8				_id;

	
#ifndef MTPT_SERVER
	NL3D::UInstance Mesh;
	NL3D::UMaterial mat;
#endif
};

#endif
