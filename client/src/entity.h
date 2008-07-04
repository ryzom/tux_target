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

#ifndef MT_ENTITY_H
#define MT_ENTITY_H


//
// Includes
//

#include <deque>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>

#include "interpolator.h"
#include "sound_manager.h"
#include "level_manager.h"
#include "../../common/lua_nel.h"
#include "../../common/constant.h"
#include "../../common/lua_utility.h"


//
// Namespaces
//

using NLMISC::CVector;
using NLMISC::CMatrix;


//
// Classes
//

class CEntityManager;

class CEntity
{
public:

	virtual ~CEntity() { delete Interpolator; Interpolator = 0; }

	enum TEntity { Unknown, Player };	// Player is bot or client
	
	void		update();
	void		renderName(bool all = false) const;
	void		close();
	void		swapOpenClose(bool ps = true);
	void		collideWhenFly(NLMISC::CVector &pos);
	void		collideWhenWater();
	bool        openClose() {return OpenClose;};
	bool		isLocal();
	void		setIsLocal(bool local);
	
	// return true if the entity collided with water or when fly
	bool		collided();

	void		playSound(CSoundManager::TSound sound);

	// accessors

	uint8				 id() const { return Id; }
//	uint8				 rank() const { return Rank; }
//	void				 rank(uint8 r) { Rank = r; }
	TEntity				 type() const { return Type; }
	const ucstring		&name() const { return Name; }
	sint32				 currentScore() const { return CurrentScore; }
	void				 setCurrentScore(sint32 score) { CurrentScore = score; }
	sint32				 lastGameScore() const { return LastGameScore; }
	void				 setLastGameScore(sint32 score) { LastGameScore = score; }
	sint32				 totalScore() const { return TotalScore; }
	void				 setTotalScore(sint32 score);
	const NLMISC::CRGBA	&color() const { return Color; }
	void				 setColor(const NLMISC::CRGBA &col);
	const string		&texture() const { return Texture; }
	void				 setTexture(const string &path);
	uint16				 ping() const { return Ping.getSmoothValue(); }
	void				 setPing(uint16 ping) { Ping.addValue(ping); }
	
	bool				 spectator() const { return Spectator; }
	void				 setSpectator(bool b) { Spectator = b; }
	bool				 ready() const { return Ready; }
	void				 setReady(bool b) { Ready = b; }
	
	void				 load3d();

	uint8				 team() const { return Team; }
	void				 setTeam(uint8 team);

	void				 fadeOpenParticleColorTo(const NLMISC::CRGBA &color,float duration);
	void				 fadeCloseParticleColorTo(const NLMISC::CRGBA &color,float duration);

	CInterpolator		&interpolator() const;

	uint8				 startPointId() const { return StartPointId; }
	void				 setStartPointId(uint8 spid);
	bool				 namePosOnScreen(CVector &res);

	bool				 fullVersion() const { return FullVersion; }

	NL3D::UInstance		CloseMesh, OpenMesh;

	NL3D::UParticleSystemInstance TraceParticleOpen;
	NL3D::UParticleSystemInstance TraceParticleClose;
	NL3D::UParticleSystemInstance ImpactParticle;

// 	NLMISC::CVector		 LastSent2MePos;
// 	NLMISC::CVector		 LastSent2OthersPos;

	float				 ArrivalTime;

	ucstring			 addChatLine;
	bool				 addOpenCloseKey;
	CCrashEvent			 addCrashEventKey;

	bool				CollidedWithWater;

private:

	NLMISC::CRGBA	OriginalColor;
	NLMISC::CRGBA   FadeOpenParticleColor;
	NLMISC::CRGBA   FadeOpenParticleStartColor;
	float           FadeOpenParticleDuration;
	float           FadeOpenParticleStartTime;

	NLMISC::CRGBA   FadeCloseParticleColor;
	NLMISC::CRGBA   FadeCloseParticleStartColor;
	float           FadeCloseParticleDuration;
	float           FadeCloseParticleStartTime;

	TEntity			 Type;
	uint8			 Id;
	uint8			 StartPointId;
//	uint8			 Rank;
	ucstring		 Name;
	string			 MeshName;
	string			 Trace;
	NLMISC::CRGBA	 Color;
	string			 Texture;
	sint32			 CurrentScore;
	sint32			 LastGameScore;
	sint32			 TotalScore;
	NLMISC::CValueSmootherTemplate<uint16> Ping;
	bool			 OpenClose; // open=true, close=false

	bool			 showCollideWhenFly;
	NLMISC::CVector	 showCollideWhenFlyPos;

	CMatrix			 ObjMatrix;
	CInterpolator	*Interpolator;
	bool			 FullVersion;

	// private ctor because only CEntityManager can create a CEntity
	CEntity();

	void reset();
	void sessionReset();
	void sessionStart();

	void init(TEntity type, const ucstring &name, sint32 totalScore, NLMISC::CRGBA &color, const string &texture, const string &meshname, bool spectator, bool isLocal, const string &trace, bool fullversion);
	void id(uint8 nid);
	void fadeOpenParticleColorUpdate();
	void fadeCloseParticleColorUpdate();

#ifdef USE_FMOD
	// contains current playing sound
	list<FMOD::Channel*>	Channels;
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// old stuff to clean



//	CSoundManager::CEntitySoundsDescriptor	SoundsDescriptor;

	sint	ParticuleOpenActivated;
	sint	ParticuleCloseActivated;

	bool	Spectator;
	bool    Ready;

	bool	Collided;
	friend class CEntityManager;

//////////////////////////////////////////////////////////////////////////

	int setMetatable(lua_State *L);
	int getUserData(lua_State *luaSession);
	int setUserData(lua_State *luaSession);
	int name(lua_State *luaSession);
	int setCurrentScore(lua_State *luaSession);
	//int setColor(lua_State *luaSession);
	int setStartPointId(lua_State *luaSession);

	LUA_BEGIN(CEntity)
		LUA_BIND(CEntity, setMetatable),
		LUA_BIND(CEntity, getUserData),
		LUA_BIND(CEntity, setUserData),
		LUA_BIND(CEntity, name),
		LUA_BIND(CEntity, setCurrentScore),
		LUA_BIND(CEntity, color), LUA_BIND(CEntity, setColor),
		LUA_BIND(CEntity, setStartPointId),
		LUA_BIND(CEntity, team), LUA_BIND(CEntity, setTeam),
	LUA_END

	LUA_ACCESSOR(CLuaRGBA, color, Color);

	LUA_NUMBER_ACCESSOR(uint8, team, Team);

private:

	uint8 Team;

	void				*LuaUserData;
	uint32				 LuaUserDataRef;
};

#endif
