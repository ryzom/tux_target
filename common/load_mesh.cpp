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
#include <nel/misc/file.h>

#include <nel/net/service.h>

#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/material.h>
#include <nel/3d/register_3d.h>

#include <nel/misc/smart_ptr.h> 

#include "load_mesh.h"
#include "editable_element_common.h"

/*
#include "bot.h"
#include "main.h"
#include "entity.h"
#include "physics.h"
#include "variables.h"
#include "entity_manager.h"
*/

//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLNET;
using namespace NLMISC;



CAutoEdge::CAutoEdge()
{
	_center = CVector::Null;
	_normal = CVector::Null;
	_faceCount = 0;
}

CAutoEdge::~CAutoEdge()
{

}

void CAutoEdge::addFace(NLMISC::CVector center,NLMISC::CVector normal)
{
	nlassert(normal.norm()>0);
	normal.normalize();
	//nlinfo("CAutoEdge::addFace #%d",_faceCount);
	//nlinfo("CAutoEdge::addFace c = %f %f %f",center.x,center.y,center.z);
	//nlinfo("CAutoEdge::addFace n = %f %f %f",normal.x,normal.y,normal.z);
	
	if(!_faceCount)
	{
		_center = center;
		_normal = normal;
		_faceCount++;
		return;
	}

	_center += center;
	CVector n = _normal /  (float)_faceCount;
	n.normalize();
	float dp = n * normal;
	if(dp<0.9f)
	{
		nlwarning("CAutoEdge::addFace new face does'nt have the same normal the previous one had");
		nlwarning("CAutoEdge::addFace new = %f %f %f / old = %f %f %f",normal.x,normal.y,normal.z,n.x,n.y,n.z);
		_normal += normal;
	}
	_faceCount++;
}

bool CAutoEdge::compute(CEditableElementCommon *aeFixedEditableElement,CEditableElementCommon *aeMoveEditableElement,CVector &translation,CVector &rotation)
{
	vector<CAutoEdge> &aeMove  = aeMoveEditableElement->AutoEdges;
	vector<CAutoEdge> &aeFixed = aeFixedEditableElement->AutoEdges;
	float dpEpsilon = 0.1f;
	float offsetEpsilon = 0.05f;
	for(uint i=0;i<aeMove.size();i++)
	{
		for(uint j=0;j<aeFixed.size();j++)
		{
			CMatrix aeFixedRotMat;
			aeFixedRotMat.identity();
			aeFixedRotMat.setRot(aeFixedEditableElement->rotation());
			CMatrix aeMoveRotMat;
			aeMoveRotMat.identity();
			aeMoveRotMat.setRot(aeMoveEditableElement->rotation());

			float dp = (aeFixedRotMat*aeFixed[i].normal()) * (aeMoveRotMat*aeMove[j].normal());
			CVector voffset = aeFixedEditableElement->transformMatrix()*aeFixed[j].center() - aeMoveEditableElement->transformMatrix()*aeMove[i].center();
			float offset = voffset.norm();
			//nlinfo(">>dp = %f , offset = %f %f %f ",dp,voffset.x,voffset.y,voffset.z);
			if(dp>(-1.0f-dpEpsilon) && dp<(-1.0f+dpEpsilon) && fabs(offset)<offsetEpsilon) //normals should be near opposed
			{
				translation = voffset;//aeFixed[j]._center - aeMove[i]._center;
				//rotation = ???
				return 	true;
			}
		}
	}
			
	return false;
}

CVector CAutoEdge::center()
{
	if(!_faceCount) return CVector::Null;
	return _center / (float)_faceCount;
}

CVector CAutoEdge::normal()
{
	if(!_faceCount) return CVector::Null;
	CVector res = _normal / (float)_faceCount;
	res.normalize();
	return res;
}




//
// Functions
//


uint32 loadMesh(const std::string &meshFileName, std::vector<NLMISC::CVector> &vertices, std::vector<NLMISC::CVector> &normals, std::vector<int> &indices, std::vector<CAutoEdge> &autoEdges, bool applyPreTransform)
{
	uint32 nbFaces = 0;

	vertices.clear();
	indices.clear();
	autoEdges.clear();

	//return 0;

	//nlinfo("loading mesh : %s",meshFileName.c_str());
	NL3D::registerSerial3d();

	if(CPath::lookup(meshFileName, false).empty())
	{
		nlwarning("Mesh '%s' is not found, can't get colission", meshFileName.c_str());
		return 0;
	}

	CMesh *m = 0;
	{
		CShapeStream ss;
		NLMISC::CIFile i(CPath::lookup(meshFileName, false).c_str());
		i.serial(ss);
		i.close();
		
		
		//CSmartPtr<IShape> is = ss.getShapePointer();
		m = (CMesh*)ss.getShapePointer();
	}
	const CMeshGeom &mg = m->getMeshGeom();

	CMatrix tmat;
	tmat.identity();

	if(applyPreTransform)
	{
		CAnimatedValueBlock avBlock;

		CVector gpos = dynamic_cast<const CAnimatedValueVector &>(m->getDefaultPos()->eval(m->getDefaultPos()->getBeginTime(),avBlock)).Value;
		CQuat grot = dynamic_cast<const CAnimatedValueBlendable<CQuat> &>(m->getDefaultRotQuat()->eval(m->getDefaultRotQuat()->getBeginTime(),avBlock)).Value;
		CVector gscale = dynamic_cast<const CAnimatedValueVector &>(m->getDefaultScale()->eval(m->getDefaultScale()->getBeginTime(),avBlock)).Value;

		tmat.setPos(gpos);
		tmat.setRot(grot);
		tmat.scale(gscale);
		//nlinfo("gpos   = %f %f %f",gpos.x,gpos.y,gpos.z);
		//nlinfo("grot   = %f %f %f",grot.x,grot.y,grot.z);
		//nlinfo("gscale = %f %f %f",gscale.x,gscale.y,gscale.z);
	}
	
	CVector center = CVector::Null;

	{
		const CVertexBuffer &vb = mg.getVertexBuffer();
		CVertexBufferRead vba;
		vb.lock (vba);
		uint32 nbv = vb.getNumVertices();
		for(uint i = 0; i < nbv; i++)
		{
			const void *vv = vba.getVertexCoordPointer(i);
			CVector v = *(CVector*)vv;
			const void *nn = vba.getNormalCoordPointer(i);
			CVector n = *(CVector*)nn;
			//		uint j;
			//		for(j = 0; j < vertices.c_str(); j++)
			//		{
			vertices.push_back(tmat * v);
			normals.push_back(n);
			center = tmat * v;
			//		}
			//		if(j)
		}
		center /= (float)nbv;
	}
	
	uint nbmb = mg.getNbMatrixBlock();
	uint aeCount = 0;
	//nlinfo(">> getNbMatrixBlock = %d ",nbmb);
	for(uint i = 0; i < nbmb; i++)
	{
		uint nbrp = mg.getNbRdrPass(i);
		//nlinfo(">> getNbRdrPass = %d ",nbrp);
		for(uint j = 0; j < nbrp; j++)
		{
			const CIndexBuffer &ib = mg.getRdrPassPrimitiveBlock(i, j);
			uint32 materialId = mg.getRdrPassMaterial(i,j);
			const CMaterial &material = m->getMaterial(materialId);
			uint kk;
			bool autoEdge = false;
			for(kk=0;kk<material.getNumUsedTextureStages();kk++)
			{
				ITexture *tex = material.getTexture(kk);
				std::string sharedName = tex->getShareName();
				//nlinfo(">> shared texture name : %s",sharedName.c_str());
				if(strstr(sharedName.c_str(),"autoedge"))
					autoEdge = true;
			}

			if(autoEdge)
			{
				CAutoEdge ae;
				autoEdges.push_back(ae);
			}
			
			CIndexBufferRead iba;
			ib.lock(iba);
			uint nbi = ib.getNumIndexes();
			uint32 *ibptr = new uint32[nbi];
			if(iba.getFormat()==CIndexBuffer::Indices16)
			{
				const uint16 *ibptrSrc = (uint16 *)iba.getPtr();
				for(uint k=0;k<nbi;k++)
					ibptr[k]=ibptrSrc[k];
			}
			else
			{
				const uint32 *ibptrSrc = (uint32 *)iba.getPtr();
				for(uint k=0;k<nbi;k++)
					ibptr[k]=ibptrSrc[k];
			}

			nlassert((nbi%3)==0);
			uint nbf = nbi/3;
			for(uint k = 0; k < nbf; k++)
			{
				uint32 ia = ibptr[k*3+0];
				uint32 ib = ibptr[k*3+1];
				uint32 ic = ibptr[k*3+2];
				indices.push_back(ia);
				indices.push_back(ib);
				indices.push_back(ic);
				if(autoEdge)
				{
					//nlinfo("face %d x=%f",k,vertices[ia].x);
					//nlinfo("face %d x=%f",k,vertices[ib].x);
					//nlinfo("face %d x=%f",k,vertices[ic].x);
					CVector faceNormal  = (normals[ia] + normals[ib] + normals[ic]) / 3;
					CVector faceCenter  = (vertices[ia] + vertices[ib] + vertices[ic]) / 3;
					autoEdges[aeCount].addFace(faceCenter,faceNormal);
				}
			}
			nbFaces+=nbf;
			//nlinfo("nb faces = %d",nbf);
			if(autoEdge)
			{
				/*
				nlinfo("auto edge = pos=%f %f %f norm=%f %f %f / mesh center = %f %f %f",
					autoEdges[aeCount].center().x, autoEdges[aeCount].center().y, autoEdges[aeCount].center().z,
					autoEdges[aeCount].normal().x, autoEdges[aeCount].normal().y, autoEdges[aeCount].normal().z,
					center.x,center.y,center.z);
					*/
				aeCount++;
			}
			delete[] ibptr;
		}
	}
	



	delete m;
	return nbFaces;
}
