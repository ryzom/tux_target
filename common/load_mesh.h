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


#ifndef MTPT_LOAD_MESH
#define MTPT_LOAD_MESH



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

uint32 loadMesh(const std::string &meshFileName, std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<int> &indices, std::vector<CAutoEdge> &autoEdges, bool applyPreTransform=false);

#endif
