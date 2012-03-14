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

#ifndef MTPT_LEVEL
#define MTPT_LEVEL


//
// Includes
//

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

#include "../../common/lua_nel.h"

#include "particles.h"
#include "module.h"
#include "start_point.h"

//
// Classes
//

class CLevel
{
public:

	CLevel(const std::string &filename);
	~CLevel();

	void display(NLMISC::CLog *log = NLMISC::InfoLog) const;

	// return true of this level instance is valid (well initialized and ready to work)
	bool			valid() const { return Valid; }

	std::string		filename() const { return FileName; }
	std::string		name() const { return Name; }
	std::string		author() const { return Author; }
	
	bool			displayStartPositions() const { return DisplayStartPositions; }
	void			displayStartPositions(bool sp);
	bool			displayLevel() const { return DisplayLevel; }
	void			displayLevel(bool l);

	void			switchStartPositions() { displayStartPositions(!DisplayStartPositions); }
	void			switchLevel() { displayLevel(!DisplayLevel); }

	void			reset();
	bool			changed() {return _changed;}
	void			changed(bool c) {_changed = c;}

	NLMISC::CVector startPosition(uint32 id);

	NLMISC::CVector cameraPosition(uint32 id);
	uint32 getCameraCount();

	CModule *getModule(uint32 id);
	CModule *getModule(std::string &name);
	uint32 getModuleCount();
	void updateModule(uint32 id,const NLMISC::CVector &pos, const NLMISC::CVector &rot,uint32 selectedBy);

	CParticles *getParticles(uint32 id);
	CParticles *getParticles(std::string &name);
	uint32 getParticlesCount();

	CStartPoint *getStartPoint(uint32 id);
	uint32 getStartPointCount();
	void updateStartPoint(uint32 id, const NLMISC::CVector &pos, const NLMISC::CVector &rot,uint32 selectedBy);

	bool execLuaCode(std::string code);
	lua_State						*luaState() {return LuaState;}

	static int getEntityByName(lua_State *L);
	static int getModuleByName(lua_State *L);
	static int getParticlesByName(lua_State *L);

	static int getEntityById(lua_State *L);
	static int getModuleById(lua_State *L);
	static int getParticlesById(lua_State *L);

	std::vector<std::pair<NLMISC::CVector, NLMISC::CQuat> >		ExternalCameras;

private:

	lua_State						*LuaState;

	// Level name
	std::string						Name;
	std::string						Author;
	
	// Path + File name of this level
	std::string						FileName;

	bool							Valid;

//	std::vector<NL3D::UInstance *>	Meshes;
	std::vector<CModule *>			Modules;
	std::vector<CParticles *>		Particles;
	std::vector<CLuaVector>			Cameras;
	//	std::vector<NL3D::UInstance *>	StartPositions;
	std::vector<CStartPoint *>		StartPoints;
	
	bool							DisplayStartPositions;
	bool							DisplayLevel;
	bool							_changed;

};

#endif
