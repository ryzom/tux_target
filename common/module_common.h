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

#ifndef MTPT_MODULE_COMMON
#define MTPT_MODULE_COMMON


//
// Includes
//

#include <vector>

#include "lua_utility.h"
#include "lua_nel.h"
#include "lunar.h"

#include "editable_element_common.h"

class CModuleCommonProxy;

//
// Classes
//

class CModuleCommon : public CEditableElementCommon
{
public:

	CModuleCommon();
	
	virtual ~CModuleCommon();

	virtual void init(const std::string &name, const std::string &shapeName, uint8 id, NLMISC::CVector position, NLMISC::CVector scale, NLMISC::CAngleAxis rotation, const NLMISC::CRGBA &color);
	void display(NLMISC::CLog *log = NLMISC::InfoLog) const;
		
	// accessors
	bool			bounce() const { return Bounce; }
	void			bounce(bool b);

	bool			collide() const { return Collide; }
	void			collide(bool c);
	
	virtual bool	enabled() const { return Enabled; }
	virtual void	enabled(bool b);
	
	sint32			score() const { return Score; }
	void			score(sint32 s) { Score = s; }

	float			accel() const { return Accel; }
	void			accel(float a) { Accel = a; }

	float			friction() const { return Friction; }
	void			friction(float f) { Friction = f; }

	std::string		name() const { return Name; }
	
	void shapeName(const std::string n) { ShapeName = n; }
	
	virtual std::string toLuaString();


protected:
	//virtual void init();

	bool				Collide;
	bool				Bounce;
	sint32				Score;
	float				Accel;
	float				Friction;
	bool				Enabled;
	std::string			LuaShapeName;
	NLMISC::CRGBA		LuaColor;
private:
	
};






#endif
