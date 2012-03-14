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

#ifdef NL_OS_WINDOWS
//#	include <winsock2.h>
#	undef min
#	undef max
#elif defined NL_OS_UNIX
#	include <unistd.h>
#	include <sys/time.h>
#	include <sys/types.h>
#endif

#include <nel/misc/path.h>
#include <nel/misc/thread.h>
#include <nel/misc/reader_writer.h>
#include <nel/misc/bit_mem_stream.h>

#include <nel/net/sock.h>
#include <nel/net/tcp_sock.h>
#include <nel/net/callback_client.h>

#include "main.h"
#include "time_task.h"
#include "mtp_target.h"
#include "network_task.h"
#include "net_callbacks.h"
#include "entity_manager.h"
#include "config_file_task.h"
#include "../../common/net_message.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//

FILE *SessionFile = 0;

CSynchronized<PauseFlags> networkPauseFlags("networkPauseFlags");
void checkNetworkPaused();

//
// Thread
//

//
// Variables
//

//
// Functions
//

void CNetworkTask::init()
{
}

void CNetworkTask::update()
{
	H_BEFORE(SockUpdate);
	Sock.update();
	H_AFTER(SockUpdate);

	//	nlinfo("Calling update %"NL_I64"u", CTime::getLocalTime());

	static int nbmsg = 0, sz = 0;
	static TTime tb = 0;
	TTime ct = CTime::getLocalTime();
	if(ct > tb + 1000)
	{
		nlinfo("received %d msg %d in %d ms", nbmsg, sz, (int)ct-tb);
		sz = nbmsg = 0;
		tb = ct;
	}

	H_BEFORE(NTLoop);
	while(Sock.dataAvailable())
	{
		H_AUTO(NTLoopOnce);

		H_BEFORE(NTLoopMsg);
		CNetMessage msg(CNetMessage::Unknown, true);
		H_AFTER(NTLoopMsg);

		H_BEFORE(NTLoopReceive);
		Sock.receive(msg);
		H_AFTER(NTLoopReceive);
		
		nbmsg++;
		sz += msg.size();

		try
		{
			uint32 nbs;
			msg.serial(nbs);
			uint8 t;
			msg.serial(t);
			msg.type((CNetMessage::TType)t);

			H_BEFORE(NTLoopCallback);
			netCallbacksHandler(msg);
			H_AFTER(NTLoopCallback);
		}
		catch(Exception &e)
		{
			nlwarning("Malformed Message type '%u' : %s", msg.Type, e.what());
		}
	}
	H_AFTER(NTLoop);
}

void CNetworkTask::release()
{
}

bool CNetworkTask::connected()
{
	return Sock.connected();
}

static void cbDisconnect(TSockId from, void *arg)
{
	// we lost the connection to the server
	CMtpTarget::getInstance().error(string("Server lost !"));
}

string CNetworkTask::connect(const CInetAddress &ip, const string &cookie)
{
	if (Sock.connected())
		return "";

	try
	{
		nlinfo ("Connection to the TCP address: %s", ip.asString().c_str());

		Sock.connect(ip);
		Sock.setDisconnectionCallback(cbDisconnect, 0);

		CNetMessage msgout(CNetMessage::Login);
		string login = CConfigFileTask::getInstance().configFile().getVar("Login").asString();
		string password = CConfigFileTask::getInstance().configFile().getVar("Password").asString();
		CRGBA color(CConfigFileTask::getInstance().configFile().getVar("EntityColor").asInt(0), CConfigFileTask::getInstance().configFile().getVar("EntityColor").asInt(1), CConfigFileTask::getInstance().configFile().getVar("EntityColor").asInt(2));
		string texture = CConfigFileTask::getInstance().configFile().getVar("EntityTexture").asString();
		networkVersion = CConfigFileTask::getInstance().configFile().getVar("NetworkVersion").asInt();
		string co = cookie;
		msgout.serial(networkVersion, co, login);
		msgout.serial(password, color, texture);
		CNetworkTask::getInstance().send(msgout);
	}
	catch (Exception &e)
	{
		return std::string("Error: ") + e.what();
	}
	
	return "";
}

static uint32 sendCount = 0;
static double sendStartCount = 0;
static double rceivStartCount = 0;
static double sendStartTime = 0;

void CNetworkTask::send(CNetMessage &msg)
{
	msg.send(&Sock);
	if(sendCount==0)
		sendStartTime = CTimeTask::getInstance().time();
	sendCount++;
	if((sendCount%100)==0 && sendCount)
	{
// TODO put the new stat
//		double dtime = CTimeTask::getInstance().time() - sendStartTime;
//		double fbsent = (double)Sock.bytesSent();
//		double fbrceiv = (double)Sock.bytesReceived();
//		double dfbsent = fbsent - sendStartCount;
//		double dfbrceiv = fbrceiv - rceivStartCount;
//		if(dtime>0.1)
//			nlinfo("during %f, up = %fB/s (%f) down = %fB/s (%f)",dtime,dfbsent/dtime,dfbsent,dfbrceiv/dtime,dfbrceiv);
//		sendStartTime = CTimeTask::getInstance().time();
//		sendStartCount = fbsent;
//		rceivStartCount = fbrceiv;
	}
}

void CNetworkTask::chat(const string &msg)
{
	// chat
	CNetMessage msgout(CNetMessage::Chat);
	string m = msg;
	msgout.serial(m);
	send(msgout);
}

void CNetworkTask::command(const string &cmd)
{
	CNetMessage msgout(CNetMessage::Command);
	string c = cmd;
	msgout.serial(c);
	send(msgout);
}

void CNetworkTask::openClose()
{
	CNetMessage msgout(CNetMessage::OpenClose);
	send(msgout);
}

void CNetworkTask::ready()
{
	nlinfo ("send ready");

	CNetMessage msgout(CNetMessage::Ready);
	send(msgout);
}

void CNetworkTask::force(const NLMISC::CVector &force)
{
	CNetMessage msgout(CNetMessage::Force);
	CVector f(force);
	msgout.serial(f);
	send(msgout);
}

void CNetworkTask::updateEditableElement(CEditableElementCommon *element)
{
	CNetMessage msgout(CNetMessage::UpdateElement);
	uint8 elementId = element->id();
	uint8 clientId = 0;
	uint8 elementType = element->type();
	msgout.serial(elementType);
	msgout.serial(elementId);
	msgout.serial(clientId);
	CVector pos = element->position();
	msgout.serial(pos);
	CVector rot = CVector(0,0,0);
	msgout.serial(rot);
	send(msgout);
}

void CNetworkTask::setEditMode(uint8 editMode)
{
	CNetMessage msgout(CNetMessage::EditMode);
	uint8 em = editMode;
	msgout.serial(em);
	send(msgout);
	
}

void CNetworkTask::stop()
{
}
