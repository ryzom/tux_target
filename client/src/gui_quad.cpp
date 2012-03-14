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

#include "3d_task.h"
#include "time_task.h"
#include "resource_manager2.h"
#include "gui_quad.h"

#include <nel/3d/u_material.h>

//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//


//
// Functions
//
	


CGuiQuad::CGuiQuad():_position(0,0,0)
{
	_width  = 1;
	_height = 1;
}

CGuiQuad::~CGuiQuad()
{
}


//screen
//y0 x0---xa------xb---x1
//   |     |      |     |
//   | f1  |  s1  |  f2 |
//   |     |      |     |
//   ya----|------|-----|
//   |     |      |     |
//   | s4  |  c   |  s2 |
//   |     |      |     |
//   yb----|------|-----|
//   |     |      |     |
//   | f4  |  s3  |  f3 |
//   |     |      |     |
//   y1------------------
//
//texture
//v0 u0---uh----u1
//   |     |     |
//   | f1  | f2  |
//   |     |     |
//   vh----|-----|
//   |     |     |
//   | f4  | f3  |
//   |     |     |
//   v1-----------
//
// c is single color zone
// f is not scaled(fixed) zone
// s is scaled zone

void CGuiQuad::render()
{
	//draw a strech quad

	float screenWidth = C3DTask::getInstance().screenWidth();
	float screenHeight = C3DTask::getInstance().screenHeight();
	float textureWidth  = 16;
	float textureHeight = 16;
	
	float x = _position.x;
	float y = _position.y;
	float width = _width;
	float height = _height;

	nlassert(width>(textureWidth/2));
	nlassert(height>(textureHeight/2));
	
	float x0 = x;
	float xa = x0 + textureWidth/2;
	float x1 = x + width;
	float xb = x1 - textureWidth/2;
	float y0 = y;
	float ya = y0 + textureHeight/2;
	float y1 = y + height;
	float yb = y1 - textureHeight/2;
	
	float u0 = 0;
	float u1 = 1;
	float uh = 0.5f;
	float v0 = 0;
	float v1 = 1;
	float vh = 0.5f;
	
	CQuadUV q;

	//f1
	q.Uv0.set(u0,v0);
	q.Uv1.set(uh,v0);
	q.Uv2.set(uh,vh);
	q.Uv3.set(u0,vh);
	q.V0.set(x0,y0,0);
	q.V1.set(xa,y0,0);
	q.V2.set(xa,ya,0);
	q.V3.set(x0,ya,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//s1
	q.Uv0.set(uh,v0);
	q.Uv1.set(uh,v0);
	q.Uv2.set(uh,vh);
	q.Uv3.set(uh,vh);
	q.V0.set(xa,y0,0);
	q.V1.set(xb,y0,0);
	q.V2.set(xb,ya,0);
	q.V3.set(xa,ya,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);

	//f2
	q.Uv0.set(uh,v0);
	q.Uv1.set(u1,v0);
	q.Uv2.set(u1,vh);
	q.Uv3.set(uh,vh);
	q.V0.set(xb,y0,0);
	q.V1.set(x1,y0,0);
	q.V2.set(x1,ya,0);
	q.V3.set(xb,ya,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//s4
	q.Uv0.set(u0,vh);
	q.Uv1.set(uh,vh);
	q.Uv2.set(uh,vh);
	q.Uv3.set(u0,vh);
	q.V0.set(x0,ya,0);
	q.V1.set(xa,ya,0);
	q.V2.set(xa,yb,0);
	q.V3.set(x0,yb,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//c
	q.Uv0.set(uh,vh);
	q.Uv1.set(uh,vh);
	q.Uv2.set(uh,vh);
	q.Uv3.set(uh,vh);
	q.V0.set(xa,ya,0);
	q.V1.set(xb,ya,0);
	q.V2.set(xb,yb,0);
	q.V3.set(xa,yb,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//s2
	q.Uv0.set(uh,vh);
	q.Uv1.set(u1,vh);
	q.Uv2.set(u1,vh);
	q.Uv3.set(uh,vh);
	q.V0.set(xb,ya,0);
	q.V1.set(x1,ya,0);
	q.V2.set(x1,yb,0);
	q.V3.set(xb,yb,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//f4
	q.Uv0.set(u0,vh);
	q.Uv1.set(uh,vh);
	q.Uv2.set(uh,v1);
	q.Uv3.set(u0,v1);
	q.V0.set(x0,yb,0);
	q.V1.set(xa,yb,0);
	q.V2.set(xa,y1,0);
	q.V3.set(x0,y1,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//s3
	q.Uv0.set(uh,vh);
	q.Uv1.set(uh,vh);
	q.Uv2.set(uh,v1);
	q.Uv3.set(uh,v1);
	q.V0.set(xa,yb,0);
	q.V1.set(xb,yb,0);
	q.V2.set(xb,y1,0);
	q.V3.set(xa,y1,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);
	
	//f3
	q.Uv0.set(uh,vh);
	q.Uv1.set(u1,vh);
	q.Uv2.set(u1,v1);
	q.Uv3.set(uh,v1);
	q.V0.set(xb,yb,0);
	q.V1.set(x1,yb,0);
	q.V2.set(x1,y1,0);
	q.V3.set(xb,y1,0);
	C3DTask::getInstance().driver().drawQuad(q,_material);	
}



float CGuiQuad::width()
{
	return _width;
}

void CGuiQuad::width(float width)
{
	_width = width;
}

float CGuiQuad::height()
{
	return _height;
}

void CGuiQuad::height(float height)
{
	_height = height;
}

CVector CGuiQuad::position()
{
	return _position;
}

void CGuiQuad::position(const CVector &position)
{
	_position = position;
}

NL3D::UMaterial CGuiQuad::material()
{
	return _material;
}

void CGuiQuad::material(NL3D::UMaterial material)
{
	_material = material;
}
