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

#ifndef MT_INTERPOLATOR_H
#define MT_INTERPOLATOR_H


//
// Includes
//

#include <deque>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/value_smoother.h>
#include <nel/misc/variable.h>


//
// Variables
//

extern NLMISC::CVariable<sint32> LCT;
void resetInterpolator();


//
// Classes
//

struct CCrashEvent
{
	CCrashEvent()
	{
		Crash = false;
		Position = NLMISC::CVector::Null;
	}

	CCrashEvent(bool c, const NLMISC::CVector &pos)
	{
		Crash = c;
		Position = pos;
	}

	bool			Crash;
	NLMISC::CVector	Position;

	CCrashEvent operator+ (const CCrashEvent &other) const;
};

struct CEntityState
{
	CEntityState();
	CEntityState(const NLMISC::CVector &position, bool openClose, const CCrashEvent &crashEvent, const NLMISC::CVector &direction = NLMISC::CVector::Null);
	CEntityState(const NLMISC::CVector &position, bool openClose, const CCrashEvent &crashEvent, ucstring chatLine, const NLMISC::CVector &direction = NLMISC::CVector::Null);

	NLMISC::CVector Position;
	bool			OpenCloseEvent;
	CCrashEvent		CrashEvent;
	ucstring		ChatLine;
	NLMISC::CVector Direction;

	CEntityState operator+ (const CEntityState &other) const;

	friend CEntityState operator* (double coef, const CEntityState &value);
	friend CEntityState operator* (const CEntityState &value, double coef);
};

template <class T> class CInterpolatorKey
{
public:

	CInterpolatorKey(const T &val, float packetdt)
	{
		Value = val;
		ServerTime = 0.0;
		PacketDT = packetdt;
	}

	virtual ~CInterpolatorKey() { }

	T &value() { return Value; }
	const T &value() const { return Value; }

	double serverTime() const { return ServerTime; }
	void setServerTime(double st) { ServerTime = st; }

	double packetDT() const { return PacketDT; }

	static T lerp(const T &a, const T &b, double pos)
	{
		return a * (1.0 - pos) + b * pos;
	}

private:

	double	ServerTime;
	T		Value;
	float	PacketDT;
};

typedef CInterpolatorKey<CEntityState> CEntityInterpolatorKey;

class CEntity;

class CInterpolator
{
public:

	CInterpolator();
	virtual ~CInterpolator() { }

	void update();
	void reset();

	void dumpPositions() const;

	// returns current values using interpolation and current time
	CCrashEvent				 crashEvent();
	bool					 openCloseEvent();
	const NLMISC::CVector	&position() const { return CurrentPosition; }
	const NLMISC::CVector	&speed() const { return CurrentSpeed; }
	float					 smoothSpeed() const { return CurrentSmoothSpeed.getSmoothValue(); }
	const NLMISC::CVector	&direction() const { return CurrentDirection; }
	const NLMISC::CVector	&smoothDirection() const { return CurrentDirection; }
	const NLMISC::CVector	&rotation() const { return CurrentRotation; }
	const NLMISC::CVector	 right() const { return CurrentDirection ^ NLMISC::CVector::K; }
	double					 facing() const { return CurrentFacing; }
	const NLMISC::CMatrix	&matrix() const { return CurrentMatrix; }

	void					 setRotation(const NLMISC::CVector &rotation) { CurrentRotation = rotation; }

	bool available() const;

	// add a key: first mean that it s the first key added from the update packet
	void					 addKey(const CEntityInterpolatorKey &key, bool first);

	// not enough key are available to interpolate correctly
	bool					 outOfKey() const { return Keys.size() < 2; }

	//double					 localTime() const { return ltNow; }

	void					 setEntity(CEntity *entity) { Entity = entity; }

	// returns keys and iterators
	const deque<CEntityInterpolatorKey> *keys(deque<CEntityInterpolatorKey>::const_iterator &key1, deque<CEntityInterpolatorKey>::const_iterator &key2) const;

private:

	CCrashEvent				 crashEvent(double time);
	bool					 openCloseEvent(double time) const;
	ucstring				 chatLine(double time) const;
	NLMISC::CVector			 position(double time) const;
	NLMISC::CVector			 speed(double time);
	NLMISC::CVector			 direction(double time);
	void					 autoAdjustLct();
	NLMISC::CVector			 rotation(double deltaTime);

	static double			 rotLerp(double rsrc, double rdst, double pos);

	double					 ServerTime;
	//double					 ltNow;
	//double					 DeltaTime;
	//double					 ltLastUpdate;
	double					 MaxKeyDiffTime;
	double					 MeanKeyDiffTime;
	float					 MaxDistBetween2Keys;

	NLMISC::CVector			 CurrentPosition;
	NLMISC::CVector			 CurrentSpeed;
	NLMISC::CValueSmoother	 CurrentSmoothSpeed;
	NLMISC::CVector			 CurrentDirection;
	NLMISC::CVector			 CurrentSmoothDirection;
	bool					 CurrentOpenCloseEvent;
	NLMISC::CVector			 CurrentRotation;
	NLMISC::CMatrix			 CurrentMatrix;
	double					 CurrentFacing;
	CCrashEvent				 CurrentCrashEvent;

	bool					 LastOpenClose;
	ucstring				 LastChatLine;
	CCrashEvent				 LastCrash;
	double					 LastRotationTime;

	CEntity					*Entity;

	deque<CEntityInterpolatorKey> Keys;
	deque<CEntityInterpolatorKey>::iterator k1, k2;
};

#endif
