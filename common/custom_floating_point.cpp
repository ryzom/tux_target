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

#include "custom_floating_point.h"


//
// Functions
//


packBit32::packBit32()
{
	currentAccess = 0;
	this->bits = 0;
}
packBit32::packBit32(uint32 bits)
{
	this->bits = bits;
	currentAccess = 32;
}

void packBit32::packBits(uint32 newBits,uint32 count)
{
	//nlassert(((uint32)1<<count)>newBits);
	if(newBits>=((uint32)1<<count))
	{
		nlwarning("((uint32)1<<count(%d))>newBits(%d)",count,newBits);
		//TODO SKEET
		newBits = (1<<count)-1;
	}
	currentAccess += count;
	nlassert(currentAccess<=32);
	bits = bits<<count;
	bits = bits | newBits;
}

void packBit32::unpackBits(uint32 &res,uint32 count)
{
	currentAccess -= count;
	nlassert(currentAccess>=0);
	uint32 mask = 0xffffffff;
	mask = mask<<count;
	res = bits & (~mask);
	bits = bits >> count;
}









#define F88_EXPONENT_MAX_VALUE 16
#define F88_MANTISSA_MAX_VALUE 16

float convert8_8fp(uint rsx,uint rdx)
{
	float mx = 1;
	bool dxisneg = false;
	sint8 dx;
	sint8 sx = rsx - (F88_MANTISSA_MAX_VALUE*2-1);
	
	nlassert(fabs((float)sx)<(F88_MANTISSA_MAX_VALUE*2));
	
	if(sx==0)
		return 0;
	dxisneg = (sx&1) != 0;
	sx = sx / 2;
	if(sx>0)
		sx--;
	dx = rdx;
	dx += F88_EXPONENT_MAX_VALUE / 2;
	if(dxisneg)
		dx = -dx;
	if(sx>0)
	{
		while(sx>0)
		{
			mx/=2;
			sx--;
		}
	}
	else if(sx<0)
	{
		while(sx<0)
		{
			mx*=2;
			sx++;
		}
	}
	return dx * mx / 20;
}


uint8 computeMantis8_8(float x,float &mx,uint8 &dx)
{

	x *= 20;
	if(x==0.0f)
	{
		mx = 0;
		dx = 0;
		return F88_MANTISSA_MAX_VALUE*2-1;
	}
	float ax = (float)fabs(x);
	sint8 sx = 0;
	mx = 1;
	if(ax>(F88_EXPONENT_MAX_VALUE))
	{
		while(ax*mx>(F88_EXPONENT_MAX_VALUE) && sx>-(F88_MANTISSA_MAX_VALUE-2))
		{
			sx--;
			mx /= 2;
		}
	}
	else
	{
		while(ax*mx<(F88_EXPONENT_MAX_VALUE) && sx<(F88_MANTISSA_MAX_VALUE-2))
		{
			sx++;
			mx *= 2;
		}
		mx /= 2;
	}
	int sdx = (int)(x * mx);
	sx = sx * 2;
	if(sdx<0)
		sx|=1;
	dx = abs(sdx);

	if( ((sx/2)==(F88_MANTISSA_MAX_VALUE-2)) && (ax*mx<(F88_EXPONENT_MAX_VALUE)) )
	{
		mx = 0;
		dx = 0;
		return F88_MANTISSA_MAX_VALUE*2-1;
	}
		
	dx -= F88_EXPONENT_MAX_VALUE/2;
	nlassert(fabs((float)sx)<(F88_MANTISSA_MAX_VALUE*2));
	return sx+(F88_MANTISSA_MAX_VALUE*2-1);	
}

float serialIn8_8fp(CNetMessage &msgin)
{
	uint8 sx;
	uint8 rdx;
	msgin.serial(sx);
	msgin.serial(rdx);
	
	return convert8_8fp(sx,rdx);
}


float serialOut8_8fp(CNetMessage &msgout,float x,uint8 &rdx,uint8 &rsx)
{
	float mx;
	uint8 dx;
	uint8 sx = computeMantis8_8(x,mx,dx);
	//sint8 dx = (sint8)(x*mx);
	msgout.serial(sx);
	msgout.serial(dx);
	rsx = sx;
	rdx = dx;
	return convert8_8fp(rsx,rdx);
}

float computeOut8_8fp(float x,uint8 &rdx,uint8 &rsx)
{
	float mx;
	uint8 dx;
	uint8 sx = computeMantis8_8(x,mx,dx);
	rsx = sx;
	rdx = dx;
	return convert8_8fp(rsx,rdx);
}

