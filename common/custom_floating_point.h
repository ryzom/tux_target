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

#ifndef MTPT_CUSTOM_FLOATING_POINT
#define MTPT_CUSTOM_FLOATING_POINT


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
