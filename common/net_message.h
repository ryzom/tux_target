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

#ifndef MT_NET_MESSAGE_H
#define MT_NET_MESSAGE_H


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
		Chat,					//	CS	SC
		Command,			//	CS
		DisplayText,		//			SC
		EndSession,		//			SC
		EditMode,			//	CS	SC
		Error,				//			SC
		Force,				//	CS
		Login,				//	CS	SC
		Logout,				//			SC
		OpenClose,		//	CS	SC
		Ready,				//	CS	SC
		EverybodyReady,//			SC
		SessionState,		//			SC
		StartSession,		//			SC
		Update,				//	CS	SC
		FullUpdate,			//			SC
		UpdateElement,	//	CS	SC
		EnableElement,	//			SC
		ExecLua,			//			SC
		Collide,				//			SC
		TimeArrival,		//			SC
		UdpLogin,			//	CS
		Level,				//	CS	SC
		MatchChallenge,	//	CS	SC
	};

	CNetMessage(TType type = Unknown, bool inputStream = false, bool reliable = true);

	TType type() const { return Type; }
	void type(TType t) { Type = t; }

	void setHeader (TType type);

	bool reliable() const { return Reliable; }

private:

	TType Type;

	bool	Reliable;

#ifdef MT_SERVER
	bool send (NLNET::CBufServer *sock, NLNET::TSockId id);
#else
	bool send (NLNET::CBufClient *sock);
#endif // MT_SERVER

	friend class CNetwork;
	friend class CNetworkTask;
};

#endif
