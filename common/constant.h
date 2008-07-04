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

#ifndef MT_CONSTANT_H
#define MT_CONSTANT_H

#ifdef NL_OS_MAC
#include <AvailabilityMacros.h>
#endif

// 3d

static const float GScale = 0.01f;

// Network

//static const float NetworkUpdateFrequency = 12.5f;														// 10 updates in 1 second
//static const float NetworkUpdatePeriod = 1.0f / NetworkUpdateFrequency;									// 0.1 second for 1 update
//static const uint32 NetworkUpdatePeriod_ms = uint32(1000.0f/NetworkUpdateFrequency);						// 100 ms for 1 update

//static const float NetworkMyUpdateFrequency_ratio = 2;													// 2 times more update
//static const float NetworkMyUpdateFrequency = NetworkUpdateFrequency * NetworkMyUpdateFrequency_ratio;	// 20 updates in 1 second
//static const float NetworkUpdatePeriod = 1.0f / NetworkUpdateFrequency;								// 0.04 second for 1 update

static const float NetworkUpdatePeriod = 40.0f / 1000.0f;								// 0.04 second for 1 update

//static const uint32 Network_myUpdatePeriod_ms = uint32(1000.0f/NetworkMyUpdateFrequency);				// 50 ms for 1 update

// do a full update after NetworkUpdate_Period delta update
//static const float Network_fullUpdatePeriod = 10*uint32(Network_myUpdateFrequency);					// send a full update after 200 updates

#endif
