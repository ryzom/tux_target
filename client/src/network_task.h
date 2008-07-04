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

#ifndef MT_NETWORK_TASK_H
#define MT_NETWORK_TASK_H


//
// Includes
//

#include <nel/misc/thread.h>
#include <nel/misc/types_nl.h>

#include <nel/net/tcp_sock.h>
#include <nel/net/inet_address.h>
#include <nel/net/login_cookie.h>
#include <nel/net/udp_sim_sock.h>

#include "entity.h"
#include "../../common/constant.h"
#include "../../common/net_message.h"
#include "../../common/editable_element_common.h"


//
// Classes
//

class CNetworkTask : public NLMISC::CSingleton<CNetworkTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render() { }
	virtual void release();

	virtual string name() const { return "CNetworkTask"; }

	// return an empty string if connected, the reason if failed
	string connect(const NLNET::CInetAddress &ip, const string &cookie = "");

	// 0=reliable 1=unreliable 2=both
	void send(CNetMessage &msg, uint8 mode = 0);

	void chat(const ucstring &msg);
	void command(const ucstring &cmd);
	void openClose();
	void ready();
	void stop();
	void setForce(const NLMISC::CVector &force);
	void updateEditableElement(CEditableElementCommon *element);
	void setEditMode(uint8 editMode);
	
	bool connected();

	NLNET::CBufClient &sock() { return Sock; }

	void sendGoogleAnalytics(const std::string &url);

private:

	NLNET::CBufClient	Sock;	// TCP connection

	NLNET::CUdpSock		UdpSock;	// UDP connection

	string				Cookie;

	bool				UdpConnectionEstablished; // True if we received some udp data from the server
};

#endif
