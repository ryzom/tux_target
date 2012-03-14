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

#include <deque>
#include <string>

#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/material.h>
#include <nel/3d/register_3d.h>

#include <nel/misc/quat.h>
#include <nel/misc/common.h>

#include <nel/3d/u_instance_material.h>

#include <nel/3d/water_height_map.h>
#include <nel/3d/water_pool_manager.h>

#include "global.h"
#include "entity.h"
#include "network_task.h"
#include "mtp_target.h"
#include "resource_manager2.h"
#include "level_manager.h"

#include <nel/misc/quat.h>
#include <nel/misc/common.h>

#include <nel/3d/u_instance_material.h>

#include <nel/3d/water_height_map.h>
#include <nel/3d/water_pool_manager.h>

#include "start_point.h"
#include "global.h"
#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "font_manager.h"
#include "config_file_task.h"
#include "../../common/load_mesh.h"

using namespace std;
using namespace NLMISC;
using namespace NL3D;





CStartPoint::CStartPoint():CStartPointCommon()
{
	mat = C3DTask::getInstance().createMaterial();
}


CStartPoint::~CStartPoint()
{
	if(!Mesh.empty())
	{
		C3DTask::getInstance().scene().deleteInstance(Mesh);
	}
}

void CStartPoint::init(const std::string &name, const std::string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CAngleAxis &rotation)
{
	CStartPointCommon::init(name, shapeName, id, position, rotation);

	ShapeName = CResourceManager::getInstance().get("col_box.shape");
	NbFaces = loadMesh(ShapeName, Vertices, Normals, Indices, AutoEdges);

	Mesh = C3DTask::getInstance().scene().createInstance (ShapeName);
	if (Mesh.empty())
	{
		nlwarning ("Can't load '%s.shape'", Name.c_str());
	}
	Mesh.setTransformMode(UTransformable::RotQuat);
	Mesh.setRotQuat(CQuat(rotation));
	Mesh.setPos(position);
	


}




//   /E--F
//  A---B|
//  | H |G
//  D---C
void CStartPoint::renderSelection()
{
	mat.setColor(CRGBA(255,255,255,255));
	mat.setZWrite(true);
	mat.setZFunc(UMaterial::always);
	CAABBox bbox;
	Mesh.getShapeAABBox(bbox);
	CAABBox tbbox = CAABBox::transformAABBox(Mesh.getMatrix(), bbox);
	
	CVector boxMin = tbbox.getMin();
	CVector boxMax = tbbox.getMax();

	CQuad quad;
	CVector a = boxMin;
	CVector b = a;
	b.x = boxMax.x;
	CVector c = a;
	c.x = boxMax.x;
	c.y = boxMax.y;
	CVector d = a;
	d.y = boxMax.y;
	
	CVector e = a;
	e.z = boxMax.z;
	CVector f = b;
	f.z = boxMax.z;
	CVector g = c;
	g.z = boxMax.z;
	CVector h = d;
	h.z = boxMax.z;
	
	CLine l;

	l.V0 = a;
	l.V1 = b;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = b;
	l.V1 = c;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = c;
	l.V1 = d;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = d;
	l.V1 = a;
	C3DTask::getInstance().driver().drawLine(l,mat);
	
	l.V0 = e;
	l.V1 = f;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = f;
	l.V1 = g;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = g;
	l.V1 = h;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = h;
	l.V1 = e;
	C3DTask::getInstance().driver().drawLine(l,mat);
	
	l.V0 = a;
	l.V1 = e;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = b;
	l.V1 = f;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = c;
	l.V1 = g;
	C3DTask::getInstance().driver().drawLine(l,mat);
	l.V0 = d;
	l.V1 = h;
	C3DTask::getInstance().driver().drawLine(l,mat);
	
	
	/*
	quad.V0 = a;
	quad.V1 = b;
	quad.V2 = c;
	quad.V3 = d;

	C3DTask::getInstance().driver().drawQuad(quad,mat);
	*/			
}


