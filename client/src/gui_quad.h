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

#ifndef MT_GUI_QUAD_H
#define MT_GUI_QUAD_H


//
// Includes
//

#include <nel/3d/u_material.h>


//
// Classes
//

class CGuiQuad
{
public:
	CGuiQuad();
	virtual ~CGuiQuad();
	virtual void render();

	float width();
	void width(float width);
	float height();
	void height(float height);
	NLMISC::CVector position();
	void position(const NLMISC::CVector &position);
	
	NL3D::UMaterial material();
	void material(NL3D::UMaterial material);
	
private:
	
	float _width;
	float _height;
	
	NLMISC::CVector _position;

	NL3D::UMaterial _material;
};


#endif
