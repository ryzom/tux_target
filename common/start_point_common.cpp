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

#include "start_point_common.h"
#include "lua_utility.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Types
//


//
// Declarations
//


//
// Variables
//


//
// Functions
//

CStartPointCommon::CStartPointCommon() : CEditableElementCommon()
{
	_type = StartPosition;
}

CStartPointCommon::~CStartPointCommon()
{
	
}

void CStartPointCommon::init(const std::string &name, const std::string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CAngleAxis &rotation)
{
	CEditableElementCommon::init(name,shapeName,id,position,CVector::Null,rotation);
}


void CStartPointCommon::update(const NLMISC::CVector &pos, const NLMISC::CVector &rot)
{
	Position = pos;
	_changed = true;
}


string CStartPointCommon::toLuaString()
{
	return toString("CVector(%f,%f,%f)",Position.x,Position.y,Position.z);
}

