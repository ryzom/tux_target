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

#ifndef MT_LOAD_MESH_H
#define MT_LOAD_MESH_H


//
// Classes
//

class CEditableElementCommon;

class CAutoEdge
{
public:
	CAutoEdge();
	~CAutoEdge();

	CAutoEdge(const CAutoEdge &ae) : _center(ae._center) , _normal(ae._normal), _faceCount(ae._faceCount) {};

	void addFace(NLMISC::CVector center,NLMISC::CVector normal);
	NLMISC::CVector center();
	NLMISC::CVector normal();
	

	static bool compute(CEditableElementCommon *aeFixedEditableElement,CEditableElementCommon *aeMoveEditableElement,NLMISC::CVector &translation, NLMISC::CVector &rotation);
protected:
private:
	NLMISC::CVector _center;
	NLMISC::CVector _normal;
	uint			_faceCount;
};


//
// Functions
//

uint32 loadMesh(const string &meshFileName, vector<NLMISC::CVector> &vertices, vector<NLMISC::CVector> &normals, vector<int> &indices, vector<CAutoEdge> &autoEdges, bool applyPreTransform=false);

#endif
