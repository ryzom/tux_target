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


#ifndef MTPT_INTERPOLATOR
#define MTPT_INTERPOLATOR


//
// Includes
//

#include <deque>
#include <string>

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_instance.h>
#include <nel/3d/u_particle_system_instance.h>


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
	CEntityState(const NLMISC::CVector &position, bool onWater, bool openClose, const CCrashEvent &crashEvent);
	CEntityState(const NLMISC::CVector &position, bool onWater, bool openClose, const CCrashEvent &crashEvent, std::string chatLine);
	
	NLMISC::CVector Position;
	bool			OnWater;
	bool			OpenCloseEvent;
	CCrashEvent		CrashEvent;
	std::string		ChatLine;

	CEntityState operator+(const CEntityState &other) const;

	friend CEntityState operator*(double coef, const CEntityState &value);
	friend CEntityState operator*(const CEntityState &value, double coef);
};

template <class T> class CInterpolatorKey
{
public:

	CInterpolatorKey(const T &val, double time)
	{
		Value = val;
		ServerTime = time;
	}

	virtual ~CInterpolatorKey() { };

	T value() const
	{
		return Value;
	}
	
	double serverTime() const
	{
		return ServerTime;
	}

	static T lerp(const T &a, const T &b, double pos)
	{
		return a * (1.0 - pos) + b * pos;
	}

protected:

	double		ServerTime;
	T			Value;	

	friend class CInterpolator;
};

typedef CInterpolatorKey<CEntityState> CEntityInterpolatorKey;

class CEntity;

class CInterpolator
{
public:

	CInterpolator(double dt);

	virtual ~CInterpolator();

	virtual void update();
	virtual void reset();

	CCrashEvent		currentCrashEvent();
	bool			currentOpenCloseEvent();
	bool			currentOnWater() const { return CurrentOnWater; }

	const NLMISC::CVector	&currentPosition() const { return CurrentPosition; }
	const NLMISC::CVector	&currentSpeed() const { return CurrentSpeed; }
	const NLMISC::CVector	&currentDirection() const { return CurrentDirection; }
	const NLMISC::CVector	&currentSmoothDirection() const { return CurrentDirection; }

	bool			available() const { return Available; }
	void			addKey(const CEntityInterpolatorKey &key);
	bool			outOfKey() const;
	double			localTime() const { return LocalTime; }

	void			dt(double ndt) { DT = ndt; }
	void			entity(CEntity *entity);

protected:

	virtual CCrashEvent		crashEvent(double time);
	virtual bool			openCloseEvent(double time) const;
	virtual std::string		chatLine(double time) const;
	virtual NLMISC::CVector	position(double time);
	virtual NLMISC::CVector	speed(double time);
	virtual NLMISC::CVector	direction(double time);
	virtual void			autoAdjustLct();
	
	std::deque<CEntityInterpolatorKey> Keys;
	double  StartTime;
	double  ServerTime;
	double  LocalTime;
	double  DeltaTime;
	double  LastUpdateTime;
	double  AddKeyTime;
	double  MaxKeyDiffTime;
	double  MeanKeyDiffTime;
	bool    Available;
	int     OutOfKeyCount;
	double	DT;
	float	MaxDistBetween2Keys;

	NLMISC::CVector CurrentPosition;
	NLMISC::CVector CurrentSpeed;
	NLMISC::CVector CurrentDirection;
	NLMISC::CVector CurrentSmoothDirection;
	bool			CurrentOnWater;
	bool			CurrentOpenCloseEvent;
	std::string     CurrentChatLine;
	
	bool LastOpenClose;
	std::string LastChatLine;
	CCrashEvent LastCrash;
	CCrashEvent CurrentCrashEvent;
	
	CEntity		*Entity;

	static double		LCT;
	static const double MinLct;
	static const double MaxLct;
};


class CLinearInterpolator : public CInterpolator
{
public:

	CLinearInterpolator(double dt);

	virtual ~CLinearInterpolator();

protected:

	virtual CCrashEvent		crashEvent(double time);
	virtual bool			openCloseEvent(double time) const;
	virtual std::string		chatLine(double time) const;
	virtual NLMISC::CVector position(double time);
	virtual NLMISC::CVector speed(double time);
	virtual NLMISC::CVector direction(double time);
	virtual void			autoAdjustLct();
};


class CExtendedInterpolator : public CLinearInterpolator
{
public:

	CExtendedInterpolator(double dt);

	virtual ~CExtendedInterpolator();
	virtual void update();

	NLMISC::CVector	rotation() const;
	NLMISC::CVector right() const;
	double			facing() const;
	NLMISC::CMatrix getMatrix() const;
	void			rotation(const NLMISC::CVector &rotation);
	
	void reset();

	static double rotLerp(double rsrc,double rdst,double pos);

private:
	
	NLMISC::CVector	rotation(double deltaTime);

	NLMISC::CVector	CurrentRotation;
	NLMISC::CMatrix	CurrentMatrix;
	double			CurrentFacing;
	double			LastRotationTime;

};

#endif
