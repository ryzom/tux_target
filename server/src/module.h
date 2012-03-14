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

#ifndef MTPT_MODULE
#define MTPT_MODULE


//
// Includes
//

#include <vector>
#include <ode/ode.h>

#include "../../common/module_common.h"

class CModuleProxy;

//
// Classes
//

class CModule : public CModuleCommon
{
public:

	CModule();
	
	virtual ~CModule();

	void initBeforeStartLevel();
		
	virtual void init(const std::string &name, const std::string &shapeName, uint8 id, const NLMISC::CVector &position, const NLMISC::CVector &scale, const NLMISC::CAngleAxis &rotation, const NLMISC::CRGBA &color);
	virtual void update(const NLMISC::CVector &pos, const NLMISC::CVector &rot);
	void update();
	

	virtual void	enabled(bool b);
	virtual void	visible(bool b);

	float bounceCoef() {return BounceCoef;}
	void bounceCoef(float c) {BounceCoef = c;}
	float bounceVel() {return BounceVel;}
	void bounceVel(float v) {BounceVel=v;}
	
	virtual void changePosition(NLMISC::CVector &pos);
	

	CModuleProxy		*luaProxy;
private:

//	virtual void init();
	void _luaInit();

	std::vector<dReal>	OdeVertices;	// 3 entries for one vertex (x,y,z)
	
	dGeomID				Geom;
	lua_State			*LuaState;
	dTriMeshDataID		triMeshDataId;
	bool				Visible;
	float				BounceCoef;
	float				BounceVel;
};






#endif
