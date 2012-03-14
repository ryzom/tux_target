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

#include <nel/misc/quat.h>
#include <nel/misc/common.h>

#include "entity.h"
#include "chat_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "font_manager.h"
#include "interpolator.h"
#include "config_file_task.h"
#include "../../common/constant.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Functions
//

CCrashEvent CCrashEvent::operator+( const CCrashEvent &other ) const
{
	return CCrashEvent( Crash || other.Crash , Position + other.Position);
}



///////////////////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////////////

CEntityState::CEntityState()
{
	Position = CVector::Null;
	OnWater = false;
	OpenCloseEvent = false;
	CrashEvent = CCrashEvent(false,CVector::Null);	
	ChatLine = "";
}

CEntityState::CEntityState(const CVector &p,bool ow, bool oce, const CCrashEvent &ce)
{
	Position = p;
	OnWater = ow;
	OpenCloseEvent = oce;
	CrashEvent = ce;
	ChatLine = "";
}

CEntityState::CEntityState(const CVector &p,bool ow, bool oce, const CCrashEvent &ce, std::string chatLine)
{
	Position = p;
	OnWater = ow;
	OpenCloseEvent = oce;
	CrashEvent = ce;
	ChatLine = chatLine;
}

CEntityState CEntityState::operator+( const CEntityState &other ) const
{
	return CEntityState( Position + other.Position , OnWater || other.OnWater, OpenCloseEvent || other.OpenCloseEvent, CrashEvent + other.CrashEvent,ChatLine);
}

CEntityState operator *( double coef, const CEntityState &value )
{
	return CEntityState( (float)coef * value.Position , value.OnWater , value.OpenCloseEvent, value.CrashEvent, value.ChatLine);
}

CEntityState operator*( const CEntityState &value, double coef )
{
	return CEntityState( (float)coef * value.Position , value.OnWater, value.OpenCloseEvent, value.CrashEvent,value.ChatLine);	
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////////////

const double CInterpolator::MinLct = 3.0  * MT_NETWORK_MY_UPDATE_PERIODE;  //less lct time doesnt improve really gameplay
const double CInterpolator::MaxLct = 12.0 * MT_NETWORK_MY_UPDATE_PERIODE; //if lct > _maxLct, the game become unplayable ...

double CInterpolator::LCT = 0.0;

CInterpolator::CInterpolator(double dt)
{
	CurrentPosition        = CVector::Null;
	CurrentSpeed           = CVector::Null;
	CurrentDirection       = CVector::Null;
	CurrentSmoothDirection = CVector::Null;
	CurrentOnWater         = false;
	CurrentOpenCloseEvent  = false;
	CurrentChatLine		   = "";
	CurrentCrashEvent  = CCrashEvent(false,CVector::Null);

	LastOpenClose = false;
	LastChatLine = "";
	LastCrash = CCrashEvent(false,CVector::Null);;
	
	Entity = 0;
	DT = dt;

	MaxDistBetween2Keys = CConfigFileTask::getInstance().configFile().getVar("InterpolatorMaxDistBetween2Keys").asFloat();

	reset();
}

CInterpolator::~CInterpolator()
{
}

void CInterpolator::reset()
{
	Keys.clear();
	StartTime      = 0.0;
	ServerTime     = 0.0;
	LocalTime      = 0.0;
	DeltaTime      = 0.0;
	LastUpdateTime = 0.0;
	Available      = false;
	LCT            = 0.0;
	MaxKeyDiffTime = 0.0;
	MeanKeyDiffTime  = 0.0;
	OutOfKeyCount  = 0;
	
//	nlinfo("reset[0x%p] (%f)",this,CTimeTask::getInstance().time());
}

void CInterpolator::entity(CEntity *entity)
{ 
	Entity = entity;
}


static int fcount = 0;
void CInterpolator::addKey(const CEntityInterpolatorKey &k)
{
	CEntityInterpolatorKey key = k;

	double time = CTimeTask::getInstance().time(); //current time
	if(!Available || StartTime==0.0)
	{
		StartTime = time;
		AddKeyTime = 0.0;
	}
	else
	{
		AddKeyTime += DT; //time the key should arrived
	}
	double localTime = time - StartTime;
	if(AddKeyTime>localTime && ReplayFile.empty())
	{
		//nlinfo("(%f)start time correction : [0x%p] : %f-%f = %f (%f)",time,this,AddKeyTime,localTime,AddKeyTime-localTime,StartTime);
		/*
		if(Entity->id()==CMtpTarget::getInstance().controler().getControledEntity())
			nlinfo("(%f)addKeyTime[0x%p] too early: %f-%f = %fms (%f)",time,this,AddKeyTime,localTime,(AddKeyTime-localTime)*1000,StartTime);
		*/
		StartTime -=  AddKeyTime - localTime;
		localTime = time - StartTime;
	}
	else
	{
		fcount++;
		/*
		if((fcount%100) == 0)
		{
			nlinfo("_lct = %f",_lct);
			nlinfo("_meanKeyDiffTime = %f",_meanKeyDiffTime);
			nlinfo("(%f)addKeyTime[0x%p] : %f-%f = %f (%f)",time,this,_addKeyTime,localTime,_addKeyTime-localTime,_startTime);
		}
		*/
		/*
		if(Entity->id()==CMtpTarget::getInstance().controler().getControledEntity())
			nlinfo("(%f)addKeyTime[0x%p] too late : %f-%f = %fms (%f)",time,this,AddKeyTime,localTime,(AddKeyTime-localTime)*1000,StartTime);
		*/
	}
	Available = true;
	key.ServerTime = AddKeyTime;
	Keys.push_back(key);

	double smoothNess = 0.9f;
	double diffKeyTime = AddKeyTime - localTime; //diff between expected key time and real key time
	double absDiffKeyTime = fabs(diffKeyTime);

	if(diffKeyTime<0.0f)
		MeanKeyDiffTime = MeanKeyDiffTime * smoothNess + absDiffKeyTime * (1.0f - smoothNess);

	if(absDiffKeyTime > MaxKeyDiffTime)
		MaxKeyDiffTime = absDiffKeyTime;
}

void CInterpolator::update()
{
	if(Keys.size()<2)
		return;

	OutOfKeyCount--;
	if(OutOfKeyCount<0)
		OutOfKeyCount=0;
	//nlinfo("outofkeycount = %d ; keysize = %d",_outOfKeyCount,_keys.size());
	if(Entity->id()==CMtpTarget::getInstance().controler().getControledEntity())
		autoAdjustLct();

	LocalTime = CTimeTask::getInstance().time() - StartTime;
	DeltaTime = LocalTime - LastUpdateTime;
	ServerTime = LocalTime - LCT;
	LastUpdateTime = LocalTime;

	CurrentPosition        = position(ServerTime);
	CurrentSpeed           = speed(ServerTime);
	CurrentDirection       = direction(ServerTime);
	CurrentSmoothDirection = CurrentDirection;

	bool ocEvent = openCloseEvent(ServerTime);
	CurrentOpenCloseEvent = ocEvent && (LastOpenClose!=ocEvent);
	LastOpenClose=ocEvent;
	
	CCrashEvent CrashEvent = crashEvent(ServerTime);
	CurrentCrashEvent.Crash = CrashEvent.Crash && (LastCrash.Crash!=CrashEvent.Crash);
	if(CurrentCrashEvent.Crash)
		CurrentCrashEvent.Position = CrashEvent.Position;
	LastCrash=CrashEvent;

	string chatLine1 = chatLine(ServerTime);
	if(chatLine1.size() && LastChatLine!=chatLine1)
	{
		CChatTask::getInstance().addLine(chatLine1);
	}
	LastChatLine = chatLine1;
	
}


bool CInterpolator::outOfKey() const
{
	return Keys.size()<2 || OutOfKeyCount>10;
}

CCrashEvent CInterpolator::currentCrashEvent()
{
	CCrashEvent res = CurrentCrashEvent;
	if(CurrentCrashEvent.Crash)
	{
		CurrentCrashEvent.Crash = false;
	}
	return res;
}

bool CInterpolator::currentOpenCloseEvent()
{
	if(CurrentOpenCloseEvent)
	{
		CurrentOpenCloseEvent = false;
		return true;
	}
	return false;
}

CCrashEvent CInterpolator::crashEvent(double time) 
{
	CEntityInterpolatorKey key = Keys.back();
	return key.value().CrashEvent;
}

string CInterpolator::chatLine(double time) const
{
	CEntityInterpolatorKey key = Keys.back();
	return key.value().ChatLine;
}

bool CInterpolator::openCloseEvent(double time) const
{
	CEntityInterpolatorKey key = Keys.back();
	return key.value().OpenCloseEvent;
}

CVector CInterpolator::position(double time)
{
	CEntityInterpolatorKey key = Keys.back();
	return key.value().Position;
}


CVector CInterpolator::speed(double time)
{
	deque<CEntityInterpolatorKey>::reverse_iterator it;
	it = Keys.rbegin();
	if(it==Keys.rend()) return(CVector::Null);
	CEntityInterpolatorKey key2 = *it;
	it++;
	if(it==Keys.rend()) return(CVector::Null);
	CEntityInterpolatorKey key1 = *it;
	return (key2.value().Position - key1.value().Position) / (float)DT;
}

CVector CInterpolator::direction(double time)
{
	deque<CEntityInterpolatorKey>::reverse_iterator it;
	it = Keys.rbegin();
	if(it==Keys.rend()) return(CVector::Null);
	CEntityInterpolatorKey key2 = *it;
	it++;
	if(it==Keys.rend()) return(CVector::Null);
	CEntityInterpolatorKey key1 = *it;
	return (key2.value().Position - key1.value().Position);
}


void CInterpolator::autoAdjustLct()
{
	LCT = MinLct;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////////////

CLinearInterpolator::CLinearInterpolator(double dt):CInterpolator(dt)
{
}

CLinearInterpolator::~CLinearInterpolator()
{
}


CCrashEvent CLinearInterpolator::crashEvent(double time) 
{
	double remainder = fmod(time,DT);
	double lerpPos = remainder / DT;
	CEntityInterpolatorKey nextKey = Keys.back();
	bool nextKeySet = false;
	for(deque<CEntityInterpolatorKey>::reverse_iterator it=Keys.rbegin();it!=Keys.rend();it++)
	{
		CEntityInterpolatorKey key = *it;
		if(key.serverTime()<time)
		{
			if(!nextKeySet)
				break;
			
			return key.value().CrashEvent;
		}
		nextKey = key;
		nextKeySet = true;
	}
	return CInterpolator::crashEvent(time);
}

bool CLinearInterpolator::openCloseEvent(double time) const
{
	double remainder = fmod(time,DT);
	double lerpPos = remainder / DT;
	deque<CEntityInterpolatorKey>::const_reverse_iterator it;
	CEntityInterpolatorKey nextKey = Keys.back();
	bool nextKeySet = false;
	for(it=Keys.rbegin();it!=Keys.rend();it++)
	{
		CEntityInterpolatorKey key = *it;
		if(key.serverTime()<time)
		{
			if(!nextKeySet)
				break;
			
			return key.value().OpenCloseEvent;
		}
		nextKey = key;
		nextKeySet = true;
	}
	return CInterpolator::openCloseEvent(time);
}

string CLinearInterpolator::chatLine(double time) const
{
	double remainder = fmod(time,DT);
	double lerpPos = remainder / DT;
	deque<CEntityInterpolatorKey>::const_reverse_iterator it;
	CEntityInterpolatorKey nextKey = Keys.back();
	bool nextKeySet = false;
	for(it=Keys.rbegin();it!=Keys.rend();it++)
	{
		CEntityInterpolatorKey key = *it;
		if(key.serverTime()<time)
		{
			if(!nextKeySet)
				break;
			
			return key.value().ChatLine;
		}
		nextKey = key;
		nextKeySet = true;
	}
	return CInterpolator::chatLine(time);
}


CVector CLinearInterpolator::position(double time)
{
	double remainder = fmod(time,DT);
	double lerpPos = remainder / DT;
	CEntityInterpolatorKey nextKey = Keys.back();
	bool nextKeySet = false;
	for(deque<CEntityInterpolatorKey>::reverse_iterator it=Keys.rbegin();it!=Keys.rend();it++)
	{
		CEntityInterpolatorKey key = *it;
		if(key.serverTime()<time)
		{
			if(!nextKeySet)
				break;

			CVector dpos = (nextKey.value().Position - key.value().Position );
			//TODO put 10 into a .cfg variable
			if(dpos.norm()>MaxDistBetween2Keys) 
			{
				nlinfo("drop key : dist between current and last  = %f",dpos.norm());
				return nextKey.value().Position;
			}
			return CEntityInterpolatorKey::lerp(key.value(),nextKey.value(),lerpPos).Position;
		}
		nextKey = key;
		nextKeySet = true;
	}
	OutOfKeyCount++;
	return CInterpolator::position(time);
}

CVector CLinearInterpolator::speed(double time)
{
	double remainder = fmod(time,DT);
	double lerpPos = remainder / DT;
	CEntityInterpolatorKey nextKey = Keys.back();
	bool nextKeySet = false;

	for(std::deque<CEntityInterpolatorKey>::reverse_iterator it=Keys.rbegin(); it!=Keys.rend(); it++)
	{
		CEntityInterpolatorKey key = *it;
		if(key.serverTime()<time)
		{
			if(!nextKeySet)
				break;
			return (nextKey.value().Position - key.value().Position)  / (float)DT;
		}
		nextKey = key;
		nextKeySet = true;
	}
	return CInterpolator::speed(time);
}

CVector CLinearInterpolator::direction(double time)
{
	double remainder = fmod(time,DT);
	double lerpPos = remainder / DT;
	CEntityInterpolatorKey nextKey = Keys.back();
	bool nextKeySet = false;
	for(deque<CEntityInterpolatorKey>::reverse_iterator it=Keys.rbegin();it!=Keys.rend();it++)
	{
		CEntityInterpolatorKey key = *it;
		if(key.serverTime()<time)
		{
			if(!nextKeySet)
				break;
			return (nextKey.value().Position - key.value().Position);
		}
		nextKey = key;
		nextKeySet = true;
	}
	return CInterpolator::direction(time);
}


void CLinearInterpolator::autoAdjustLct()
{
	double lct;
	lct = MeanKeyDiffTime * 4;
	lct = max(lct,MinLct);
	lct = min(lct,MaxLct);
//	lct = _meanKeyDiffTime + 2 * _dt;
	LCT = lct;
//	_lct = 4 * _dt;
}




///////////////////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////////////

CExtendedInterpolator::CExtendedInterpolator(double dt) : CLinearInterpolator(dt)
{
	reset();
}

CExtendedInterpolator::~CExtendedInterpolator()
{	
}

void CExtendedInterpolator::reset()
{
	CInterpolator::reset();
	CurrentRotation        = CVector::Null;
	CurrentFacing          = 0.0;
	LastRotationTime       = 0.0;	
}

void CExtendedInterpolator::update()
{
	CInterpolator::update();
	
	CurrentRotation = rotation(DeltaTime);

	nlassert(Entity!=0);
	if(Entity->openClose())
	{
		if(Entity->isLocal() && ReplayFile.empty() )
		{
			CurrentMatrix.identity();
			CurrentMatrix.rotateZ((float)CurrentFacing);
			CurrentMatrix.rotateX(0.5f * (float)NLMISC::Pi * (1.0f - CurrentRotation.x));
		}
		else
		{
			CurrentMatrix.identity();
			CurrentMatrix.rotateZ((float)CurrentFacing);
			float instantRotX = (float)(-0.5f * CurrentSpeed.z * DeltaTime / GScale - (float)NLMISC::Pi * 0.08f);
			float lpos = (float)(5 * DeltaTime / 1.0f);
			CurrentRotation.x = (float)rotLerp(CurrentRotation.x, instantRotX, lpos);
			CurrentMatrix.rotateX(CurrentRotation.x);
		}
	}
	else
	{
		CVector speed = CurrentSpeed;
		speed.z = 0.0f;
		if(speed.norm() > 0.01f*GScale)
		{
			CVector nspeed = speed;
			nspeed.normalize();
			CVector right = nspeed ^ CVector(0,0,1);
			float angleX = (float)( - speed.norm() * DeltaTime / GScale);
			right = CurrentMatrix.inverted().mulVector(right);
			CurrentMatrix.rotate(CQuat(CAngleAxis(right, angleX)));
		}
	}
	
	//_currentMatrix.identity();//TODO remove this line
	CurrentMatrix.setPos(CurrentPosition);


	CVector dir = CurrentDirection;//smoothDirection();
	dir.z = 0.0f;
	if(dir.norm() > 0.001f && (CMtpTarget::getInstance().State == CMtpTarget::eGame || CMtpTarget::getInstance().isSpectatorOnly()))
	{
		dir.normalize();
		float instantFacing = (float)acos(dir.y);
		if(dir.x>0.0f)
			instantFacing = - instantFacing;
		instantFacing += (float)NLMISC::Pi;
		double lpos = 20 * DeltaTime / 1.0f;
		CurrentFacing = rotLerp(CurrentFacing, instantFacing, lpos);
		if(Entity->isLocal())
			CurrentFacing = instantFacing;
	}
/*
	_currentMatrix.identity();
	_currentMatrix.setPos(_currentPosition);
	_currentMatrix.rotate(CQuat(CAngleAxis(right(), _currentRotation.x)));
*/	
/*
	if(DisplayDebug)
	{
		nlinfo("lct = %g, in queue %2d ct %g st %g (dt = %g)", Lct, PositionKeys.size(), LocalTime, ServerTime, deltaTime);
	}
*/	
}


CVector CExtendedInterpolator::right() const
{
	return CurrentDirection ^ CVector(0,0,1);
}

CVector CExtendedInterpolator::rotation() const
{
	return CurrentRotation;
}

double CExtendedInterpolator::facing() const
{
	return CurrentFacing;
}

void CExtendedInterpolator::rotation(const CVector &rotation)
{
	CurrentRotation = rotation;
}


CMatrix CExtendedInterpolator::getMatrix() const
{
	return CurrentMatrix;
}

CVector CExtendedInterpolator::rotation(double deltaTime)
{
	if(Entity->openClose())
	{
		double instantRotX = -0.5f * CurrentSpeed.z * deltaTime / GScale - (double)NLMISC::Pi * 0.08f;
		double lpos = 0.1f * deltaTime / 1.0f;
		CurrentRotation.x = (float )rotLerp(CurrentRotation.x, instantRotX, lpos);
	}
	return CurrentRotation;
}

double  CExtendedInterpolator::rotLerp(double rsrc,double rdst,double pos)
{
	double nrsrc = fmod((double)rsrc, 2*NLMISC::Pi) + 2*NLMISC::Pi;
	nrsrc = fmod((double)nrsrc, 2*NLMISC::Pi);
	double nrdst = fmod((double)rdst, 2*NLMISC::Pi) + 2 * NLMISC::Pi ;
	nrdst = fmod((double)nrdst, 2*NLMISC::Pi);
	if(nrsrc<nrdst)
	{
		if(fabs(nrdst - nrsrc) > NLMISC::Pi)
			nrdst -= 2*NLMISC::Pi;
	}
	else
	{
		if(fabs(nrsrc - nrdst) > NLMISC::Pi)
			nrsrc -= 2*NLMISC::Pi;
	}
	double ipos = 1.0f - pos;

	double res = (double) (nrdst * pos + nrsrc * ipos);
	//nlinfo("%f => %f (%f) = %f",nrsrc,nrdst,pos,res);
	return res;
}
