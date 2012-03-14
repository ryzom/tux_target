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
// This class manages the network connection with the server
//

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

#include "entity.h"
#include "../../common/net_message.h"
#include "../../common/constant.h"
#include "../../common/editable_element_common.h"


//
// Constants
//

static const float NetVirtualFrameDuration = MT_NETWORK_MY_UPDATE_PERIODE;


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

	virtual std::string name() const { return "CNetworkTask"; }
	
	// return an empty string if connected, the reason if failed
	std::string connect(const NLNET::CInetAddress &ip, const std::string &cookie = "");
	
	void send(CNetMessage &msg);

	void chat(const std::string &msg);
	void command(const std::string &cmd);
	void openClose();
	void ready();
	void stop();
	void force(const NLMISC::CVector &force);
	void updateEditableElement(CEditableElementCommon *element);
	void setEditMode(uint8 editMode);
		
	bool connected();

	NLNET::CBufClient &sock() { return Sock; }

	uint32 networkVersion;

private:
	
	NLNET::CBufClient	Sock;
};

#endif
