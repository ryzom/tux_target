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

#ifndef MT_CUSTOM_FLOATING_POINT_H
#define MT_CUSTOM_FLOATING_POINT_H


//
// Includes
//

#include "net_message.h"

//
// Classes
//

class packBit32
{
public:
	packBit32();
	packBit32(uint32 bits);
	void packBits(uint32 newBits,uint32 count);
	void unpackBits(uint32 &res,uint32 count);
	
	uint32 bits;
	sint32 currentAccess;
protected:
private:
};


//
// Functions
//

float convert8_8fp(uint rsx,uint rdx);
uint8 computeMantis8_8(float x,float &mx,uint8 &dx);
float computeOut8_8fp(float x,uint8 &rdx,uint8 &rsx);
float serialIn8_8fp(CNetMessage &msgin);
float serialOut8_8fp(CNetMessage &msgout,float x,uint8 &rdx,uint8 &rsx);


#endif
