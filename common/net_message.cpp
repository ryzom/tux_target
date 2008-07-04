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

#include <nel/misc/variable.h>
#include <nel/misc/time_nl.h>

#include "net_message.h"

#ifndef MT_SERVER
#	include "../client/src/graph.h"
#endif


//
// Namespaces
//

using namespace NLMISC;

//
// Variables
//

CVariable<sint32> nbsmsg("mtp", "nbsmsg", "Display the number of message sent", 0, 60*5);
CVariable<sint32> sizesmsg("mtp", "sizesmsg", "Display the size of message sent", 0, 60*5);


//
// Functions
//

CNetMessage::CNetMessage(TType type, bool inputStream, bool reliable) : CMemStream(inputStream), Reliable(reliable)
{
	if(!inputStream)
		setHeader(type);
}

void CNetMessage::setHeader(TType type)
{
	static uint32 nextsendvalue = 0;
	serial(nextsendvalue);
	nextsendvalue++;

	nlassert(type < 256);
	Type = type;
	uint8 t = (uint8)Type;
	serial(t);
}


#ifdef MT_SERVER
bool CNetMessage::send (NLNET::CBufServer *sock, NLNET::TSockId id)
#else
bool CNetMessage::send (NLNET::CBufClient *sock)
#endif // MT_SERVER
{
	nlassert(sock);

//	nlinfo("real send %"NL_I64"d", CTime::getLocalTime());
//	nldebug("NET: Send a message type %hu size %u", (uint16)buffer()[4], this->length());

#ifdef MT_SERVER
	sock->send(*this, id);
	sock->flush(id);
#else
	sock->send(*this);
	sock->flush();
#endif // MT_SERVER

	static TTime o = CTime::getLocalTime();
	TTime n = CTime::getLocalTime();
	static sint32 nbs = 0, ss = 0;
	nbs++;
	ss += length();

#ifndef MT_SERVER
	NbSMsgGraph.addValue (1.0f);
	SizeSMsgGraph.addValue (float(length()));
#endif

	if(n-o > 1000)
	{
		nbsmsg = nbs;
		sizesmsg = ss;
		o = n;
		nbs = 0;
		ss = 0;
	}

	return true;
}
