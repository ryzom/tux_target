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
// This class manages an entity (player or bot)
//

#ifndef MTPT_MODULE
#define MTPT_MODULE


//
// Includes
//

#include <deque>
#include <string>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>

#include "global.h"


//
// Includes
//

#include <deque>
#include <string>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>

#include "sound_manager.h"
#include "interpolator.h"
#include "../../common/module_common.h"
#include "module_lua_proxy.h"


//
// Namespaces
//

using NLMISC::CVector;
using NLMISC::CMatrix;


class CModuleProxy;

//
// Classes
//

class CModule : public CModuleCommon
{
public:
	CModule();
	virtual ~CModule();
	
	virtual void enabled(bool b);

	virtual void init(const std::string &name, const std::string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation, const NLMISC::CRGBA &color);
	virtual void renderSelection();
	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot);
	
	virtual void position(const NLMISC::CVector &pos) {Position = pos; _changed = true; Mesh.setPos(pos);}
	virtual NLMISC::CVector position() {return CEditableElementCommon::position();}
	
	const NLMISC::CRGBA	&color() const { return Color; }
	void  color(const NLMISC::CRGBA &col);
	void luaInit();
	
	CModuleProxy *LuaProxy;
private:
	NLMISC::CRGBA	Color;
};

#endif
