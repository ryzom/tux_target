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

#ifndef MTPT_CONSTANT
#define MTPT_CONSTANT

// ugly UGLY! C++ likes static const float and not #define!!!
#define MT_NETWORK_UPDATE_FREQUENCE 10.0f
#define MT_NETWORK_UPDATE_PERIODE (1.0f/MT_NETWORK_UPDATE_FREQUENCE)
#define MT_NETWORK_UPDATE_PERIODE_MS ((uint32 )(1000.0f/MT_NETWORK_UPDATE_FREQUENCE))

#define MT_NETWORK_MY_UPDATE_FREQUENCE_RATIO 2
#define MT_NETWORK_MY_UPDATE_FREQUENCE (MT_NETWORK_UPDATE_FREQUENCE * MT_NETWORK_MY_UPDATE_FREQUENCE_RATIO)
#define MT_NETWORK_MY_UPDATE_PERIODE (1.0f/MT_NETWORK_MY_UPDATE_FREQUENCE)
#define MT_NETWORK_MY_UPDATE_PERIODE_MS ((uint32 )(1000.0f/MT_NETWORK_MY_UPDATE_FREQUENCE))

//do a full update after MT_NETWORK_UPDATE__PERIODE delta update
#define MT_NETWORK_FULL_UPDATE_PERIODE (10*(uint32)MT_NETWORK_MY_UPDATE_FREQUENCE)  //every 10 sec

#endif
