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

#ifndef NL_OS_WINDOWS
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
#include "graph.h"
#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "network_task.h"
#include "net_callbacks.h"
#include "entity_manager.h"
#include "config_file_task.h"
#include "../../common/async_job.h"
#include "../../common/net_message.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NLNET;


//
// Variables
//

//FILE *SessionFile = 0;

CVariable<sint32> nbrmsg("mtp", "nbrmsg", "Display the number of message received", 0, 60*5);
CVariable<sint32> sizermsg("mtp", "sizermsg", "Display the size of message received", 0, 60*5);

static CVariable<bool> UseUdp("mtp", "UseUdp", "use tcp or udp to send position", true, 0, true);

CVector LatestForce = CVector::Null;


//
// Functions
//

void CNetworkTask::init()
{
	UdpConnectionEstablished = false;
}

void CNetworkTask::update()
{
	H_BEFORE(SockUpdate);
	Sock.update();
	H_AFTER(SockUpdate);

	static TTime o = CTime::getLocalTime();
	TTime n = CTime::getLocalTime();
	static sint32 nbr = 0, sr = 0;
	if(n-o > 1000)
	{
		nbrmsg = nbr;
		sizermsg = sr;
		o = n;
		nbr = 0;
		sr = 0;
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

		nbr++;
		sr += msg.length();

		NbRMsgGraph.addValue (1.0f);
		SizeRMsgGraph.addValue (float(msg.length()));

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
			nlwarning("Malformed Message type %u : %s", msg.Type, e.what());
		}
	}
	H_AFTER(NTLoop);

	// udp update

	uint8 packet[1000];
	uint32 psize;

	while (UdpSock.dataAvailable())
	{
		psize = 1000;
		try
		{
			CNetMessage msg(CNetMessage::Unknown, true);
			UdpSock.receive (packet, psize);
			msg.fill(packet, psize);

			nbr++;
			sr += psize;

			UNbRMsgGraph.addValue (1.0f);
			USizeRMsgGraph.addValue (float(psize));

			try
			{
				uint32 nbs;
				msg.serial(nbs);
				uint8 t;
				msg.serial(t);
				msg.type((CNetMessage::TType)t);
				netCallbacksHandler(msg);

				if(!UdpConnectionEstablished)
				{
					UdpConnectionEstablished = true;
					//nlinfo("Received an UDP packet from server, we can communicate with this cnx");
				}
			}
			catch(Exception &e)
			{
				nlwarning("Malformed Message type %u : %s", msg.Type, e.what());
			}
		}
		catch ( Exception& e )
		{
			nlwarning ("Received failed: %s", e.what());
		}
	}
}

void CNetworkTask::release()
{
	if(connected())
		Sock.disconnect();
}

bool CNetworkTask::connected()
{
	return Sock.connected();
}

static void cbDisconnect(TSockId from, void *arg)
{
	// we lost the connection to the server
	CMtpTarget::instance().error("ErrServerLost");
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
		Cookie = cookie;
		msgout.serial(Cookie);
		CNetworkTask::instance().send(msgout);

		if(UseUdp)
		{
			nlinfo ("Connection to the UDP address: %s", ip.asString().c_str());
			UdpSock.connect(ip);
		}
	}
	catch (Exception &e)
	{
		return string("Error: ") + e.what();
	}

	return "";
}

void CNetworkTask::send(CNetMessage &msg, uint8 mode)
{
	if((mode == 1 || mode == 2) && UseUdp && UdpConnectionEstablished)
	{
		uint32 size = msg.length();
		UNbSMsgGraph.addValue (1.0f);
		USizeSMsgGraph.addValue (float(size));
		CSock::TSockResult res = UdpSock.send (msg.buffer(), size, false);
		if(res != CSock::Ok)
		{
			nlwarning("Cannot send a message in udp, stop using UDP %d", res);
			UseUdp = false;
		}
		if(mode==2)
			msg.send(&Sock);
	}
	else
	{
		msg.send(&Sock);
	}

	if((mode == 1 || mode == 2) && UseUdp && !UdpConnectionEstablished)
	{
		// try a new packet
		CNetMessage udpmsgout(CNetMessage::UdpLogin);
		udpmsgout.serial (Cookie);
		uint32 size = udpmsgout.length();
		UNbSMsgGraph.addValue (1.0f);
		USizeSMsgGraph.addValue (float(size));
		CSock::TSockResult res = UdpSock.send (udpmsgout.buffer(), size, false);
		if(res != CSock::Ok)
		{
			nlwarning("Cannot send UdpLogin message, stop using UDP %d", res);
			UseUdp = false;
		}
		//nldebug ("Sent a UdpLogin message");
	}
}

void CNetworkTask::chat(const ucstring &msg)
{
	// chat
	CNetMessage msgout(CNetMessage::Chat);
	ucstring m = msg;
	msgout.serial(m);
	send(msgout);
}

void CNetworkTask::command(const ucstring &cmd)
{
	CNetMessage msgout(CNetMessage::Command);
	ucstring c = cmd;
	msgout.serial(c);
	send(msgout);
}

//DEBUG
extern double sentCTRL;

void CNetworkTask::openClose()
{
	sentCTRL = CTimeTask::instance().time();
	// Since we send this message 2 times, in UDP *AND* TCP, we need to track on the server if the message is already received or not.
	// The goal is to try to remove the LAG on the CTRL that happen sometimes.
	static uint8 next = 0;
	next++;
	nlwarning("CTRL: PRESSED, msg sent %f %u", sentCTRL, next);
	CNetMessage msgout(CNetMessage::OpenClose);
	msgout.serial(next);
	send(msgout, 2);
}

void CNetworkTask::ready()
{
	//nlinfo ("send ready");

	CNetMessage msgout(CNetMessage::Ready);
	send(msgout);
}

void CNetworkTask::setForce(const NLMISC::CVector &force)
{
	LatestForce = force;

/*
	CNetMessage msgout(CNetMessage::Force);
	CVector f(force);
	msgout.serial(f);
	send(msgout, false);
*/

	//ForceGraph.addOneValue(f.norm());

/*
	static FILE *fp = 0;
	if(DisplayDebug==3 && !fp)
	{
		fp = fopen("force.csv", "wt");
	}

	if(DisplayDebug==3 && fp)
	{
		fprintf(fp, "%f;%f;%f;%f;%f\n", CTimeTask::instance().deltaTime(), f.x, f.y, f.z, f.norm());
	}

	if(DisplayDebug!=3 && fp)
	{
		fclose(fp);
		fp = 0;
	}
*/
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

uint32 GAFirstRandom = 0;
uint32 GAFirstTime = 0;

void CNetworkTask::sendGoogleAnalytics(const string &url)
{
	string dest = CConfigFileTask::instance().tempDirectory()+"ga_request";
	string request;
	request  = "http://www.google-analytics.com/__utm.gif?utmwv=1";
	uint32 random = uint32(frand(1.0f)*2147483647);
	if(GAFirstRandom == 0) GAFirstRandom = random;
	request += "&utmn="+toString(random);	// Number
	request += "&utmcs=UTF-8";										// Code String
	request += toString("&utmsr=%ux%u", C3DTask::instance().screenWidth(), C3DTask::instance().screenHeight());	// Screen Resolution
	request += "&utmsc=32-bit";	// Screen Capability
	request += "&utmul="+CI18N::getCurrentLanguageCode();	// User Language
	request += "&utmje="+string(C3DTask::instance().fullscreen()?"1":"0");	// Java Enabled, in our case, it's fullscreen or not
	request += "&utmfl=-";				// FLash
	//request += "&utmcn=1";				// C N
	//request += "&utmdt="; // Title	// Document Title
	request += "&utmhn=mtp.ploki.info";	// Host Name
	request += "&utmr=-";				// Referrer
	request += "&utmp="+url;			// Path
	request += "&utmac=UA-150324-10";	// ACcount
	request += "&utmcc="; // CooCie __utma%3D61038448.1380021965.1203089948.1203089948.1203089948.1%3B%2B__utmb%3D61038448%3B%2B__utmc%3D61038448%3B%2B__utmz%3D61038448.1203087682.1.1.utmcsr%3D(direct)%7Cutmccn%3D(direct)%7Cutmcmd%3D(none)%3B%2B";

	if(GAFirstTime == 0) GAFirstTime = CTime::getSecondsSince1970();

	request += "__utma%3D61038448."+toString(GAFirstRandom)+"."+toString(GAFirstTime)+"."+toString(GAFirstTime)+"."+toString(GAFirstTime)+".1%3B%2B";
	request += "__utmb%3D61038448%3B%2B";
	request += "__utmc%3D61038448%3B%2B";
	request += "__utmz%3D61038448."+toString(GAFirstTime)+".1.1.utmccn%3D(direct)%7Cutmcsr%3D(direct)%7Cutmcmd%3D(none)%3B%2B";

	//request += "";
	//nlinfo("GA: '%s'", request.c_str());
	//downloadFile(request, dest);
#ifdef NL_OS_WINDOWS
	string useragent = "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.12) Gecko/20080201 Firefox/2.0.0.12";
#elif defined(NL_OS_MAC)
	string useragent = "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.5; en-US; rv:1.8.1.12) Gecko/20080201 Firefox/2.0.0.12";
#elif defined(NL_OS_UNIX)
	string useragent = "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.12) Gecko/20080201 Firefox/2.0.0.12";
#endif
	CAsyncJob::instance().addDownloadJob(request, dest, useragent);
}
