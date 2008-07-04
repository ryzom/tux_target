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

#ifndef MT_PARTICLE_COMMON_H
#define MT_PARTICLE_COMMON_H


//
// Includes
//

#include "lua_utility.h"
#include "lua_nel.h"
#include "lunar.h"

#include "editable_element_common.h"

//
// Classes
//

class CParticlesCommon : public CEditableElementCommon
{
public:

	CParticlesCommon();
	
	virtual ~CParticlesCommon();

	//void init(const string &name, const string &fileName, uint8 id, NLMISC::CVector position, NLMISC::CVector scale, NLMISC::CAngleAxis rotation, bool show, bool started);
	void display(NLMISC::CLog *log = NLMISC::InfoLog) const;
		
	virtual bool	enabled() const { return Enabled; }
	virtual void	enabled(bool b);

	void fileName(const string n) { ShapeName = n; }

protected:

	bool				Enabled;
	string				LuaShapeName;
	bool				LuaShow;
	bool				LuaStarted;
};

#endif
