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


//
// Includes
//

#include "stdpch.h"

#include <nel/misc/quat.h>
#include <nel/misc/common.h>
#include <nel/misc/variable.h>

#include "graph.h"
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

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

CVariable<sint32> LCT("mtp", "LCT", "lct in second", 0);
static CVariable<uint32> NbInterpolatorKeys("mtp", "NbInterpolatorKeys", "Adjust the LCT to have this number of key in the array", 2, 0, true);
double gtStartTime = 0.0;
double stAddKeyTime = 0.0;


//
// Functions
//

void resetInterpolator()
{
	gtStartTime = 0.0;
	stAddKeyTime = 0.0;
}

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
//	OnWater = false;
	OpenCloseEvent = false;
	CrashEvent = CCrashEvent(false,CVector::Null);
	ChatLine.clear();
	Direction = CVector::Null;
}

CEntityState::CEntityState(const CVector &p, /*bool ow,*/ bool oce, const CCrashEvent &ce, const NLMISC::CVector &direction)
{
	Position = p;
//	OnWater = ow;
	OpenCloseEvent = oce;
	CrashEvent = ce;
	ChatLine.clear();
	Direction = direction;

	if(gtStartTime==0.0)
	{
		gtStartTime = CTimeTask::instance().time();
		stAddKeyTime = 0.0;
	}
}

CEntityState::CEntityState(const CVector &p, /*bool ow,*/ bool oce, const CCrashEvent &ce, ucstring chatLine, const NLMISC::CVector &direction)
{
	Position = p;
//	OnWater = ow;
	OpenCloseEvent = oce;
	CrashEvent = ce;
	ChatLine = chatLine;
	Direction = direction;

	if(gtStartTime==0.0)
	{
		gtStartTime = CTimeTask::instance().time();
		stAddKeyTime = 0.0;
	}
}

CEntityState CEntityState::operator+( const CEntityState &other ) const
{
	return CEntityState( Position + other.Position, /*OnWater || other.OnWater,*/ OpenCloseEvent || other.OpenCloseEvent, CrashEvent + other.CrashEvent,ChatLine, Direction + other.Direction);
}

CEntityState operator *( double coef, const CEntityState &value )
{
	return CEntityState( (float)coef * value.Position, /*value.OnWater,*/ value.OpenCloseEvent, value.CrashEvent, value.ChatLine, (float)coef * value.Direction);
}

CEntityState operator*( const CEntityState &value, double coef )
{
	return CEntityState( (float)coef * value.Position, /*value.OnWater,*/ value.OpenCloseEvent, value.CrashEvent,value.ChatLine, (float)coef * value.Direction);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////////////

CInterpolator::CInterpolator(/*double dt*/) : CurrentSmoothSpeed(32)
{
	CurrentPosition        = CVector::Null;
	CurrentSpeed           = CVector::Null;
	CurrentDirection       = CVector::Null;
	CurrentSmoothDirection = CVector::Null;
	CurrentOpenCloseEvent  = false;
	CurrentCrashEvent  = CCrashEvent(false,CVector::Null);

	CurrentRotation        = CVector::Null;
	CurrentFacing          = 0.0;
	LastRotationTime       = 0.0;

	LastOpenClose = false;
	LastCrash = CCrashEvent(false,CVector::Null);

	Entity = 0;

	MaxDistBetween2Keys = CConfigFileTask::instance().configFile().getVar("InterpolatorMaxDistBetween2Keys").asFloat();

	Keys.clear();
//	gtStartTime    = 0.0;
	ServerTime     = 0.0;

	k1 = Keys.begin();
	k2 = Keys.begin();
	reset();
}

bool CInterpolator::available() const
{
	return gtStartTime != 0.0;
}

void CInterpolator::reset()
{
	////Keys.clear();
	////gtStartTime    = 0.0;
	////ServerTime     = 0.0;
//	ltNow      = 0.0;
//	DeltaTime      = 0.0;
//	ltLastUpdate = 0.0;
	////Available      = false;
//	LCT            = 0;
//	MaxKeyDiffTime = 0.0;
//	MeanKeyDiffTime  = 0.0;

	CurrentSmoothSpeed.reset();
	CurrentRotation        = CVector::Null;
	CurrentFacing          = 0.0;
	LastRotationTime       = 0.0;

	//nldebug("reset[0x%p] (%f)",this,CTimeTask::instance().time());
}

void CInterpolator::addKey(const CEntityInterpolatorKey &k, bool first)
{
	CEntityInterpolatorKey key = k;

	double gtNow = CTimeTask::instance().time();

	if(Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
	{
		PacketDTGraph.addOneValue (float(k.packetDT()*1000.0f));
		static double gtLast = 0;
		if(gtLast != 0)
		{
			DTGraph.addOneValue(float(gtNow - gtLast) * 1000.0f);
		}
		gtLast = gtNow;
	}
/*	else
	{
		nlinfo("new key %f %f %f", k.value().Position.x, k.value().Position.y, k.value().Position.z);
	}
*/
	if(first)
	{
		if(gtStartTime==0.0)
		{
			gtStartTime = gtNow;
			stAddKeyTime = 0.0;
		}
		else
		{
			stAddKeyTime += k.packetDT();
		}
	}

// gt = global time (seconds since the launch of the game)
// lt = local time (seconds since the beginning of this level)

//	double ltNow = gtNow - gtStartTime;
//	if(stAddKeyTime > stNow && ReplayFile.empty())
//	{
//		//nlinfo("(%f)start time correction : [0x%p] : %f-%f = %f (%f)",time,this,AddKeyTime,localTime,AddKeyTime-localTime,StartTime);
//		if(Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
//			nldebug("(%f)addKeyTime[0x%p] too early: %f-%f = %fms (%f)",gtNow,this,stAddKeyTime,stNow,(stAddKeyTime-stNow)*1000,gtStartTime);
//
//		gtStartTime -=  stAddKeyTime - ltNow;
//		ltNow = gtNow - gtStartTime;
//	}

	key.setServerTime(stAddKeyTime);
	Keys.push_back(key);

//	if(/*DisplayDebug==3 &&*/ Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
//		nldebug("add a key at the end %f %f", key.serverTime(), ServerTime);

	//double smoothness = 0.9f;
	//double diffKeyTime = stAddKeyTime - stNow; // diff between expected key time and real key time
	//double absDiffKeyTime = fabs(diffKeyTime);

	//if(diffKeyTime<0.0f)
	//	MeanKeyDiffTime = MeanKeyDiffTime * smoothness + absDiffKeyTime * (1.0f - smoothness);

	//if(absDiffKeyTime > MaxKeyDiffTime)
	//	MaxKeyDiffTime = absDiffKeyTime;
}

void CInterpolator::dumpPositions() const
{
	double gtNow = CTimeTask::instance().time();
	double ltNow = gtNow - gtStartTime;

	//DeltaTime = ltNow - ltLastUpdate;
	double DeltaTime = CTimeTask::instance().deltaTime();
	double st = ltNow - (double(LCT)/1000.0);

	nlwarning("%f %f %f %f %f", gtNow, ltNow, DeltaTime, st, ServerTime);

	nlwarning("nbkey %d", Keys.size());
	for(deque<CEntityInterpolatorKey>::const_iterator k = Keys.begin(); k != Keys.end(); k++)
	{
		nlwarning("%f %f / %f %f %f", (*k).serverTime(), (*k).value().Position.x, (*k).value().Position.y, (*k).value().Position.z);
	}
	if(k1 != Keys.end() && k2 != Keys.end())
	{
		nlwarning("%f %f / %f %f %f", (*k1).serverTime(), (*k1).value().Position.x, (*k1).value().Position.y, (*k1).value().Position.z);
		nlwarning("%f %f / %f %f %f", (*k2).serverTime(), (*k2).value().Position.x, (*k2).value().Position.y, (*k2).value().Position.z);

		double remainder = ServerTime - double((*k1).serverTime());
		double lerpPos = remainder / (*k2).packetDT();
		CVector pos = CEntityInterpolatorKey::lerp((*k1).value(), (*k2).value(), lerpPos).Position;
		nlwarning("%f %f %f", pos.x, pos.y, pos.z);
	}
	else
	{
		if(k1 == Keys.end()) nlwarning("k1 end");
		if(k2 == Keys.end()) nlwarning("k2 end");
	}
}

void CInterpolator::update()
{
//	if(Entity->id()==CMtpTarget::instance().controler().getControledEntity())
//		autoAdjustLct();

	double gtNow = CTimeTask::instance().time();
	double ltNow = gtNow - gtStartTime;

	//DeltaTime = ltNow - ltLastUpdate;
	double DeltaTime = CTimeTask::instance().deltaTime();
	ServerTime = ltNow - (double(LCT)/1000.0);
	//ltLastUpdate = ltNow;

	if(ReplayFile.empty())
	{
		// remove out of date keys
		do
		{
			k1 = Keys.begin();
			if(k1 == Keys.end()) break;
			k2 = k1 + 1;
			if(k2 == Keys.end()) break;
			if((*k1).serverTime() >= ServerTime || (*k2).serverTime() >= ServerTime) break;
			//		if(/*DisplayDebug==3 &&*/ Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
			//			nldebug("remove first key %f %f %f", (*k1).serverTime(), ServerTime, (*k2).serverTime());
			Keys.pop_front();
		}
		while(true);

		// adjust the LCT depending on how many keys are in the array
		if(Keys.size() > NbInterpolatorKeys) LCT = LCT - 1;
		else if(Keys.size() < NbInterpolatorKeys) LCT = LCT + 1;
	}
	else
	{
		// move to the good key, one per update or we ll miss some open/close event
		if(k2 != Keys.end() && (*k2).serverTime() < ServerTime)
		{
			k2++;
			k1 = k2 - 1;
		}

		if(k1 != Keys.begin() && (*k1).serverTime() >= ServerTime)
		{
			k1--;
			k2 = k1 + 1;
		}

		LCT = 0;
	}

	if(Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
	{
		NbKeysGraph.addOneValue (float(Keys.size()));
		LCTGraph.addOneValue (float(LCT));
	}

	if(/*DisplayDebug==3 &&*/ Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
	{
		//nldebug("%s nbkey: %d time: start %f local %f delta %f server %f", Entity->name().toUtf8().c_str(), Keys.size(), gtStartTime, ltNow, DeltaTime, ServerTime);
	}

	if(k1 == Keys.end() || (k1+1) == Keys.end())
	{
//		if(/*DisplayDebug==3 &&*/ Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
		{
			//nldebug("***** not enough keys lct: %d nbkey: %d time: start %f local %f delta %f server %f", LCT.get(), Keys.size(), gtStartTime, ltNow, DeltaTime, ServerTime);
		}
		return;
	}
	CurrentPosition        = position(ServerTime);
	CurrentSpeed           = speed(ServerTime);
	CurrentSmoothSpeed.addValue(speed(ServerTime).norm());
	CurrentDirection       = direction(ServerTime);
	CurrentSmoothDirection = CurrentDirection;
	CurrentRotation		   = rotation(ServerTime);

	if(/*DisplayDebug==3 &&*/ Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
	{
//		nldebug("pos %f/%f/%f speed %f/%f/%f dir %f/%f/%f rot %f/%f/%f", CurrentPosition.x, CurrentPosition.y, CurrentPosition.z, CurrentSpeed.x, CurrentSpeed.y, CurrentSpeed.z, CurrentDirection.x, CurrentDirection.y, CurrentDirection.z, CurrentRotation.x, CurrentRotation.y, CurrentRotation.z);
	}

	// debug position using CSV file
/*	if(Entity->id()==CMtpTarget::instance().controler().Camera.getFollowedEntity())
	{
		static FILE *fp = 0;
		if(DisplayDebug==3 && !fp)
		{
			fp = fopen("trace.csv", "wt");
		}
		if(DisplayDebug==3 && fp)
		{
			fprintf(fp, "%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n", ltNow, (*(Keys.begin())).serverTime(), ServerTime, CurrentFacing, (*(Keys.begin())).value().Position.x, (*(Keys.begin())).value().Position.y, (*(Keys.begin())).value().Position.z, CurrentPosition.x, CurrentPosition.y, CurrentPosition.z, CurrentDirection.x, CurrentDirection.y, CurrentDirection.z);
		}
		if(DisplayDebug!=3 && fp)
		{
			fclose(fp);
			fp = 0;
		}
	}*/

	bool ocEvent = openCloseEvent(ServerTime);
	CurrentOpenCloseEvent = ocEvent && (LastOpenClose!=ocEvent);
	LastOpenClose=ocEvent;

	CCrashEvent CrashEvent = crashEvent(ServerTime);
	CurrentCrashEvent.Crash = CrashEvent.Crash && (LastCrash.Crash!=CrashEvent.Crash);
	if(CurrentCrashEvent.Crash)
		CurrentCrashEvent.Position = CrashEvent.Position;
	LastCrash=CrashEvent;

	ucstring chatLine1 = chatLine(ServerTime);
	if(chatLine1.size() && LastChatLine!=chatLine1)
	{
		CChatTask::instance().addLine(chatLine1);
	}
	LastChatLine = chatLine1;

	nlassert(Entity!=0);
	if(Entity->openClose())
	{
		if(Entity->isLocal() && ReplayFile.empty() )
		{
			CurrentMatrix.identity();
			CurrentMatrix.rotateZ((float)CurrentFacing);
			CurrentMatrix.rotateX(0.5f * (float)Pi * (1.0f - CurrentRotation.x));
		}
		else
		{
			CurrentMatrix.identity();
			CurrentMatrix.rotateZ((float)CurrentFacing);
 			float instantRotX = (float)(-0.5f * CurrentSpeed.z * DeltaTime / GScale - (float)Pi * 0.08f);
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

	CurrentMatrix.setPos(CurrentPosition);
}

CCrashEvent CInterpolator::crashEvent()
{
	CCrashEvent res = CurrentCrashEvent;
	if(CurrentCrashEvent.Crash)
	{
		CurrentCrashEvent.Crash = false;
	}
	return res;
}

bool CInterpolator::openCloseEvent()
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
	//if(Keys.size()<2) return (*Keys.begin()).value().CrashEvent;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	double remainder = time - double((*k1).serverTime());
	double lerpPos = remainder / (*k2).packetDT();
	return (*k1).value().CrashEvent;
}

bool CInterpolator::openCloseEvent(double time) const
{
	//if(Keys.size()<2) return (*Keys.begin()).value().OpenCloseEvent;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	double remainder = time - double((*k1).serverTime());
	double lerpPos = remainder / (*k2).packetDT();
	return (*k1).value().OpenCloseEvent;
}

ucstring CInterpolator::chatLine(double time) const
{
	//if(Keys.size()<2) return (*Keys.begin()).value().ChatLine;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	double remainder = time - double((*k1).serverTime());
	double lerpPos = remainder / (*k2).packetDT();
	return (*k1).value().ChatLine;
}

CVector CInterpolator::position(double time) const
{
	//if(Keys.size()<2) return (*Keys.begin()).value().Position;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	double remainder = time - double((*k1).serverTime());
	double lerpPos = remainder / (*k2).packetDT();
	return CEntityInterpolatorKey::lerp((*k1).value(), (*k2).value(), lerpPos).Position;
}

CVector CInterpolator::speed(double time)
{
	//if(Keys.size()<2) return CVector::Null;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	return ((*k2).value().Position - (*k1).value().Position) / float((*k1).packetDT());
}

CVector CInterpolator::direction(double time)
{
	//if(Keys.size()<2) return CVector::Null;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	(*k2).value().Direction = (*k2).value().Position - (*k1).value().Position;

	double remainder = time - double((*k1).serverTime());
	double lerpPos = remainder / (*k2).packetDT();
	return CEntityInterpolatorKey::lerp((*k1).value(), (*k2).value(), lerpPos).Direction;
}

CVector CInterpolator::rotation(double time)
{
	CVector dir = CurrentDirection;
	dir.z = 0.0f;
	if(dir.norm() > 0.001f && (CMtpTarget::instance().State == CMtpTarget::eGame || CMtpTarget::instance().spectator()))
	{
		dir.normalize();
// 		float instantFacing = (float)acos(dir.y);
// 		if(dir.x > 0.0f) instantFacing = -instantFacing;
//		CurrentFacing = instantFacing + float(Pi);
		CurrentFacing = float(atan2(dir.y, dir.x) + Pi/2.0);
	}

	//if(Keys.size()<2) return CurrentRotation;
	nlassert(k1 != Keys.end() && k2 != Keys.end());

	if(!Entity->isLocal() && Entity->openClose())
	{
  		double instantRotX = -0.5f * CurrentSpeed.z * (*k2).packetDT() / GScale - (double)Pi * 0.08f;
  		double lpos = 0.1f * (*k2).packetDT() / 1.0f;
  		CurrentRotation.x = (float )rotLerp(CurrentRotation.x, instantRotX, lpos);
	}
	return CurrentRotation;
}

double CInterpolator::rotLerp(double rsrc, double rdst, double pos)
{
	double nrsrc = fmod((double)rsrc, 2*Pi) + 2*Pi;
	nrsrc = fmod((double)nrsrc, 2*Pi);
	double nrdst = fmod((double)rdst, 2*Pi) + 2 * Pi;
	nrdst = fmod((double)nrdst, 2*Pi);
	if(nrsrc<nrdst)
	{
		if(fabs(nrdst - nrsrc) > Pi)
			nrdst -= 2*Pi;
	}
	else
	{
		if(fabs(nrsrc - nrdst) > Pi)
			nrsrc -= 2*Pi;
	}
	double ipos = 1.0f - pos;

	double res = (double) (nrdst * pos + nrsrc * ipos);
	//nlinfo("%f => %f (%f) = %f",nrsrc,nrdst,pos,res);
	return res;
}

void CInterpolator::autoAdjustLct()
{
/*	sint32 lct;
	lct = sint32(MeanKeyDiffTime * 4.0 * 1000.0);
	lct = max(lct, MinLct);
	lct = min(lct, MaxLct);
	LCT = lct;*/
}

const deque<CEntityInterpolatorKey> *CInterpolator::keys(deque<CEntityInterpolatorKey>::const_iterator &key1, deque<CEntityInterpolatorKey>::const_iterator &key2) const
{
	key1 = k1;
	key2 = k2;
	return &Keys;
}
