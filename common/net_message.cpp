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

#include "net_message.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;


//
// Functions
//

CNetMessage::CNetMessage(TType type, bool inputStream) : CMemStream(inputStream)
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


#ifdef MTPT_SERVER
bool CNetMessage::send (NLNET::CBufServer *sock, NLNET::TSockId id)
#else
bool CNetMessage::send (NLNET::CBufClient *sock)
#endif // MTPT_SERVER
{
	nlassert(sock);

//	nlinfo("real send %"NL_I64"d", CTime::getLocalTime());
//	nldebug("NET: Send a message type %hu size %u", (uint16)buffer()[4], this->length());

#ifdef MTPT_SERVER
	sock->send(*this, id);
	sock->flush(id);
#else
	sock->send(*this);
	sock->flush();
#endif // MTPT_SERVER

	return true;
}
