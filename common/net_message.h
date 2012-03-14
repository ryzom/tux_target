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
// This class manages the loading of a level
//

#ifndef MT_NET_MESSAGE
#define MT_NET_MESSAGE


//
// Includes
//

#include <nel/misc/types_nl.h>
#include <nel/misc/mem_stream.h>

#include <nel/net/tcp_sock.h>
#include <nel/net/buf_client.h>
#include <nel/net/buf_server.h>


//
// Classes
//

//
// Main class that manage buffer messages, receive and send
//

class CNetMessage : public NLMISC::CMemStream
{
public:

	enum TType
	{
		Unknown = 0,
		// alphabetic order
		Chat,				//  1 CS SC
		Command,			//  2 CS
		DisplayText,		//  3    SC
		EndSession,			//  4    SC
		EditMode,			//  5 CS SC
		Error,				//  6    SC
		Force,				//  7 CS
		Login,				//  8 CS SC
		Logout,				//  9    SC
		OpenClose,			// 10 CS SC
		Ready,				// 11 CS SC
		EverybodyReady,		// 12    SC
		RequestCRCKey,		// 13 CS SC
		RequestDownload,	// 14 CS SC
		SessionState,		// 15    SC
		StartSession,		// 16    SC
		Update,				// 17 CS SC
		UpdateOne,			// 18    SC
		FullUpdate,			// 19    SC
		UpdateElement,		// 20 CS SC
		EnableElement,		// 21    SC
		ExecLua,			// 22    SC
		CollideWhenFly,		// 23    SC
		TimeArrival,		// 24    SC
	};

	CNetMessage(TType type = Unknown, bool inputStream = false);

	TType type() const { return Type; }
	void type(TType t) { Type = t; }

	void setHeader (TType type);

private:

	TType Type;

#ifdef MTPT_SERVER
	bool send (NLNET::CBufServer *sock, NLNET::TSockId id);
#else
	bool send (NLNET::CBufClient *sock);
#endif // MTPT_SERVER

	friend class CNetwork;
	friend class CNetworkTask;
};

#endif
