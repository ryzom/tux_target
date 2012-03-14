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
// This is the main class that manages all other classes
//

#ifndef MTPT_GUI_STRETCHED_QUAD_H
#define MTPT_GUI_STRETCHED_QUAD_H


//
// Includes
//

#include <nel/3d/u_material.h>


//
// Classes
//

class CGuiStretchedQuad
{
public:
	CGuiStretchedQuad();
	virtual ~CGuiStretchedQuad();
	virtual void render();
	
	NLMISC::CVector size();
	void size(const NLMISC::CVector &size);
	NLMISC::CVector position();
	void position(const NLMISC::CVector &position);
	
	NL3D::UMaterial material();
	void material(NL3D::UMaterial material);

	void color(NLMISC::CRGBA color);
	NLMISC::CRGBA color();
	void offset(const NLMISC::CVector &offset);
	NLMISC::CVector offset();

	void stretched(bool stretched);
	bool stretched();
	
private:
	
	NLMISC::CVector _size;
	
	NLMISC::CVector _position;

	NL3D::UMaterial _material;

	NLMISC::CRGBA _color;
	NLMISC::CVector _offset;

	bool _stretched;
};


#endif
