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

#ifndef MT_LEVEL_H
#define MT_LEVEL_H


//
// Includes
//

#include "mtp_target.h"
#include "start_point.h"
#include "level_manager.h"
#include "../../common/lua_nel.h"


//
// Classes
//

class CGate;
class CModule;
class CParticles;

class CLevel
{
public:

	CLevel(const string &filename);
	~CLevel();

	void display(NLMISC::CLog *log = NLMISC::InfoLog) const;

	void update();

	// return true of this level instance is valid (well initialized and ready to work)
	bool			valid() const { return Valid; }

	const string &filename() const { return FileName; }
	
	bool			displayStartPositions() const { return DisplayStartPositions; }
	void			displayStartPositions(bool sp);
	bool			displayLevel() const { return DisplayLevel; }
	void			displayLevel(bool l);

	void			switchStartPositions() { displayStartPositions(!DisplayStartPositions); }
	void			switchLevel() { displayLevel(!DisplayLevel); }

	void			reset();

	NLMISC::CVector startPosition(uint32 id);

	NLMISC::CVector cameraPosition(uint32 id);
	uint32 getCameraCount();

	CModule *module(uint32 id);
	CModule *module(string &name);
	uint32 moduleCount();
	void updateModule(uint32 id,const NLMISC::CVector &pos, const NLMISC::CVector &rot,uint32 selectedBy);

	CParticles *particles(uint32 id);
	CParticles *particles(string &name);
	uint32 particlesCount();

	CStartPoint *getStartPoint(uint32 id);
	uint32 getStartPointCount();
	void updateStartPoint(uint32 id, const NLMISC::CVector &pos, const NLMISC::CVector &rot, uint32 selectedBy);

	bool execLua(const string &code);

	static int entityByName(lua_State *L);
	static int moduleByName(lua_State *L);
	static int particlesByName(lua_State *L);

	static int entityById(lua_State *L);
	static int moduleById(lua_State *L);
	static int particlesById(lua_State *L);

	vector<pair<NLMISC::CVector, NLMISC::CQuat> >		ExternalCameras;

	const ucstring &name() const;
	const ucstring &info() const;

private:

	void loadLevel();
	void deleteLevel();

	// Path + File name of this level
	string						FileName;

	bool						Valid;

	vector<CGate*>				Gates;
	vector<CModule *>			Modules;
	vector<CParticles *>		Particles;
	vector<CLuaVector>			Cameras;
	vector<CStartPoint *>		StartPoints;

	bool						DisplayStartPositions;
	bool						DisplayLevel;

	void setCameraMinDistFromStartPointToMove(float val) { CMtpTarget::instance().controler().Camera.minDistFromStartPointToMove(val); }
	float cameraMinDistFromStartPointToMove() { return CMtpTarget::instance().controler().Camera.minDistFromStartPointToMove(); }

	void setCameraMinDistFromStartPointToMoveVerticaly(float val) { CMtpTarget::instance().controler().Camera.minDistFromStartPointToMoveVerticaly(val); }
	float cameraMinDistFromStartPointToMoveVerticaly() { return CMtpTarget::instance().controler().Camera.minDistFromStartPointToMoveVerticaly(); }

	mutable ucstring			Name;
	ucstring Info;

public:

	LUA_BEGIN(CLevel)
		LUA_BIND(CLevel, setInfo),
		LUA_BIND(CLevel, addExternalCamera),
		LUA_BIND(CLevel, addGate),
		LUA_BIND(CLevel, addModule),
		LUA_BIND(CLevel, addStartPoint),
		LUA_BIND(CLevel, addCamera),
		LUA_BIND(CLevel, nbCameras),
		LUA_BIND(CLevel, setWater),
		LUA_BIND(CLevel, setSky),
		LUA_BIND(CLevel, setFog),
		LUA_BIND(CLevel, setSun),
		LUA_BIND(CLevel, timeout), LUA_BIND(CLevel, setTimeout),
		LUA_BIND(CLevel, hasBonusTime), LUA_BIND(CLevel, setHasBonusTime),
		LUA_BIND(CLevel, crashedClientCollide), LUA_BIND(CLevel, setCrashedClientCollide),
		LUA_BIND(CLevel, clientBounceCoef), LUA_BIND(CLevel, setClientBounceCoef),
		LUA_BIND(CLevel, clientBounceVel), LUA_BIND(CLevel, setClientBounceVel),
		LUA_BIND(CLevel, cameraMinDistFromStartPointToMove), LUA_BIND(CLevel, setCameraMinDistFromStartPointToMove),
		LUA_BIND(CLevel, cameraMinDistFromStartPointToMoveVerticaly), LUA_BIND(CLevel, setCameraMinDistFromStartPointToMoveVerticaly),
		LUA_BIND(CLevel, advancedLevel), LUA_BIND(CLevel, setAdvancedLevel),
 	LUA_END

	BOOL_ACCESSOR(hasBonusTime, HasBonusTime);
	BOOL_ACCESSOR(crashedClientCollide, CrashedClientCollide);
	BOOL_ACCESSOR(advancedLevel, AdvancedLevel);

	NUMBER_ACCESSOR(float, timeout, Timeout);

	NUMBER_ACCESSOR(float, clientBounceCoef, ClientBounceCoef);
	NUMBER_ACCESSOR(float, clientBounceVel, ClientBounceVel);

	LUA_NUMBER_ACCESSOR(float, cameraMinDistFromStartPointToMove, CameraMinDistFromStartPointToMove);
	LUA_NUMBER_ACCESSOR(float, cameraMinDistFromStartPointToMoveVerticaly, CameraMinDistFromStartPointToMoveVerticaly);

	int addExternalCamera(lua_State *L);
	int addGate(lua_State *L);
	int addModule(lua_State *L);
	int addStartPoint(lua_State *L);
	int addCamera(lua_State *L);
	int nbCameras(lua_State *L);
	int setWater(lua_State *L);
	int setSky(lua_State *L);
	int setFog(lua_State *L);
	int setSun(lua_State *L);
	int setInfo(lua_State *L);
};

#endif
