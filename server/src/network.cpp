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

#include <nel/misc/thread.h>
#include <nel/misc/reader_writer.h>

#include <nel/net/sock.h>
#include <nel/net/service.h>
#include <nel/net/buf_server.h>
#include <nel/net/listen_sock.h>

#include "main.h"
#include "physics.h"
#include "network.h"
#include "net_callbacks.h"
#include "entity_manager.h"
#include "../../common/constant.h"
#include "../../common/net_message.h"
#include "../../common/custom_floating_point.h"


//
// Namespaces
//

using namespace std;
using namespace NLNET;
using namespace NLMISC;


//
// Variables
//

//
// Thread
//

//
// Variables
//

//
// Functions
//

static void cbDisconnection(TSockId from, void *arg)
{
	for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
	{
		nlassert(*it);
		if((*it)->type() != CEntity::Client)
			continue;
		CClient *c = (CClient *)(*it);
		nlassert(c->sock());
			
		if(c->sock() != from)
			continue;
		CEntityManager::getInstance().remove(c->id());
		break;
	}
}

static void cbConnection(TSockId from, void *arg)
{
	// new player comes in
	CEntityManager::getInstance().addClient(from);
}

CNetwork::CNetwork() : BufServer(0)
{
	Version = 0;
}

void CNetwork::init()
{
	nlassert(!BufServer);

	BufServer = new CBufServer();
	BufServer->init(NLNET::IService::getInstance()->ConfigFile.getVar("TcpPort").asInt());
	BufServer->setConnectionCallback(cbConnection, 0);
	BufServer->setDisconnectionCallback(cbDisconnection, 0);

	updateCount = 0;

	MinDeltaToSendFullUpdate = NLNET::IService::getInstance()->ConfigFile.getVar("MinDeltaToSendFullUpdate").asFloat();
	DisableNetworkOptimization = NLNET::IService::getInstance()->ConfigFile.getVar("DisableNetworkOptimization").asInt()!=0;
	Version = NLNET::IService::getInstance()->ConfigFile.getVar("NetworkVersion").asInt();
	NextUpdateTime = 0;

	
	bool publicChatBot = NLNET::IService::getInstance()->ConfigFile.getVar("publicChatBot").asInt()!=0;
	if(publicChatBot)
	{
		ChatSock.connect(CInetAddress("mtp-target.dyndns.org",4000));
		ChatSock.setNonBlockingMode(true);
	}
	
}

void CNetwork::reset()
{
	updateCount = 0;	
}


//static FILE *fp = 0;
//static int rpos;
//static int updateCount2 = 0;

void CNetwork::update()
{
	H_AUTO(CNetworkUpdate);

	// update only network every 50ms
	static TTime tb = 0;
	TTime ct = CTime::getLocalTime();
	if(ct < tb + 40) return;
	tb = ct;

	uint8 ch[257];
	uint32 len = 256;
	if(ChatSock.connected() && ChatSock.dataAvailable())
	{
		ChatSock.receive( ch,len);
		ch[len]='\0';
		nlinfo("recv : '%s'",ch);
		string rcv = (char *)ch;
		string publicChatBotLogin = IService::getInstance()->ConfigFile.getVar("publicChatBotIdentity").asString(0);
		//login
		if(rcv.find(string("\n<Mtp> Login:"))!=string::npos)
		{
			sendToPublicChat(publicChatBotLogin);
		}
		//password
		else if(rcv.find(string("<Mtp> Password:"))!=string::npos && ch[0]==255 && ch[1]==251)//TODO hmmhmm :)
		{
			string publicChatBotPassword = IService::getInstance()->ConfigFile.getVar("publicChatBotIdentity").asString(1);
			string publicChatBotChannel= IService::getInstance()->ConfigFile.getVar("publicChatBotIdentity").asString(2);
			sendToPublicChat(publicChatBotPassword);
			sendToPublicChat("join "+publicChatBotChannel);
		}
		//channel
		else if(ch[0]=='<' && rcv.find(string("<Mtp>"))!=0 && rcv.find("<"+publicChatBotLogin+">")!=0 )
		{
			bool command = false;
			for(uint i = 0; i < IService::getInstance()->ConfigFile.getVar("publicChatForwardTo").size(); i++)
			{
				string nickName = IService::getInstance()->ConfigFile.getVar("publicChatForwardTo").asString(i);
				string cmdHeader = "<"+nickName+"> .";
				uint32 pos = rcv.find(cmdHeader);
				if(pos==0)
				{
					string cmd = rcv.substr(cmdHeader.size());
					if(cmd.size())
					{
						cmd = cmd.substr(0,cmd.size()-1);//remove last \n
						CNetwork::getInstance().sendChat(nickName+" executed: /"+cmd);
						ICommand::execute(cmd, *InfoLog);
					}
					command = true;
					break;
				}
			}
			if(!command)
			{
				sendChat(rcv,false);
				tellToPublicChat(rcv);
			}
		}
		//tell
		else if(ch[0]=='<' && rcv.find(string("<Mtp>"))==0)
		{
			for(uint i = 0; i < IService::getInstance()->ConfigFile.getVar("publicChatForwardTo").size(); i++)
			{
				string nickName = IService::getInstance()->ConfigFile.getVar("publicChatForwardTo").asString(i);
				string tellStr = "<Mtp> "+nickName+" tells you: ";
				string askStr = "<Mtp> "+nickName+" asks you: ";
				uint32 tellPos = rcv.find(tellStr);
				uint32 askPos = rcv.find(askStr);
				uint32 pos = 1;
				string msgStr;
				if(tellPos==0)
				{
					msgStr = tellStr;
					pos = 0;
				}
				if(askPos==0)
				{
					msgStr = askStr;
					pos = 0;
				}

				if(pos==0)
				{
					string rawMsgStr = rcv.substr(msgStr.size());
					string tellMsgStr = "<"+nickName+"> "+rawMsgStr;
					string cmdHeader = "<"+nickName+"> .";
					pos = tellMsgStr.find(cmdHeader);
					if(pos==0)
					{
						string cmd = tellMsgStr.substr(cmdHeader.size());
						if(cmd.size())//remove last \n
						{
							cmd = cmd.substr(0,cmd.size()-1);//remove last \n
							CNetwork::getInstance().sendChat(nickName+" executed: /"+cmd);
							ICommand::execute(cmd, *InfoLog);
						}
					}
					else
					{
						sendChat(tellMsgStr,false);
						tellToPublicChat(tellMsgStr);
						//forwardToPublicChat(tellMsgStr);
					}
					break;
				}
			}
			
		}
	}


	static uint8 pingnb = 0;
	
	static TTime tsttime = 0;
	TTime t0 = CTime::getLocalTime ();
	TTime updatedt = 0;
	if(tsttime)
		updatedt = t0 - tsttime;
	tsttime = t0;
	if(updatedt>100)
		nlinfo("time before 2 network update : %d>100ms",updatedt);

	{
		{
			TTime currentTime = CTime::getLocalTime();
			unsigned int i;
			CEntityManager::EntityConstIt it;
			for(i=0,it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++,i++)
			{
				const dReal *pos = dBodyGetPosition((*it)->Body);
				CVector npos;
				
				npos.x = (float)pos[0];
				npos.y = (float)pos[1]; 
				npos.z = (float)pos[2];
				
				uint8 eid = (*it)->id();
				uint16 ping = (*it)->Ping.getSmoothValue();
				//msgout.serial(eid);
				
				CVector dpos = npos - (*it)->LastSent2OthersPos;
				if(dpos.norm()>MinDeltaToSendFullUpdate)
				{
					updateCount=0;
					break;
				}
			}
		}

		if((updateCount%MT_NETWORK_FULL_UPDATE_PERIODE)==0 || DisableNetworkOptimization)
	//	if(updateCount==0)
		{
			CNetMessage msgout(CNetMessage::FullUpdate);
			
			{
				msgout.serial(pingnb);

				TTime currentTime = CTime::getLocalTime();
				
				for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
				{
					const dReal *pos = dBodyGetPosition((*it)->Body);
					
					(*it)->Pos.x = (float)pos[0];
					(*it)->Pos.y = (float)pos[1]; 
					(*it)->Pos.z = (float)pos[2];
					
					uint8 eid = (*it)->id();
					uint16 ping = (*it)->Ping.getSmoothValue();
					msgout.serial(eid);
					msgout.serial((*it)->Pos, ping);
					
					if((*it)->type() != CEntity::Bot)
					{
						(*it)->LastSentPing.push(make_pair(pingnb, currentTime));
//						nlinfo("*********** send the ping %u %"NL_I64"u", pingnb, currentTime);
					}
					
					(*it)->LastSent2MePos = (*it)->Pos;
					(*it)->LastSent2OthersPos = (*it)->Pos;
				}
			}
			
			UpdatePacketSize = msgout.length();
			
			CNetwork::getInstance().send(msgout);
			pingnb++;
		}
		else if((updateCount%MT_NETWORK_MY_UPDATE_FREQUENCE_RATIO)==0)
		{
			CNetMessage msgout(CNetMessage::Update);
			
			{
				msgout.serial(pingnb);
				
				TTime currentTime = CTime::getLocalTime();
				
				for(list<uint8>::iterator it1=CEntityManager::getInstance().IdUpdateList.begin();it1!=CEntityManager::getInstance().IdUpdateList.end();it1++)
				{
					uint8 eid = *it1;
					for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
					{
						if((*it)->id()==eid)
						{
							const dReal *pos = dBodyGetPosition((*it)->Body);
							
							(*it)->Pos.x = (float)pos[0];
							(*it)->Pos.y = (float)pos[1]; 
							(*it)->Pos.z = (float)pos[2];
							
							uint8 eid = (*it)->id();
							uint16 ping = (*it)->Ping.getSmoothValue();
							//msgout.serial(eid);

							CVector dpos = (*it)->Pos - (*it)->LastSent2OthersPos;
							
							uint8 sx;
							uint8 dx;
							CVector sendDPos;
							packBit32 pb32;

							sendDPos.x = computeOut8_8fp(dpos.x,dx,sx);
							pb32.packBits(dx,4);
							pb32.packBits(sx,6);
							sendDPos.y = computeOut8_8fp(dpos.y,dx,sx);
							pb32.packBits(dx,4);
							pb32.packBits(sx,6);
							sendDPos.z = computeOut8_8fp(dpos.z,dx,sx);
							pb32.packBits(dx,4);
							pb32.packBits(sx,6);

							msgout.serial(pb32.bits);

							/*
							if((*it)->id()==rpos && dpos.z!=0)
							//if(dpos.z!=0)
							{
								sint8 dsx = sx;// - (*it)->LastSentSX;
								//fwrite(&dx,1,1,fp);
								fwrite(&dsx,1,1,fp);
								fflush(fp);
								(*it)->LastSentSX = sx;
							}
							*/
							
							
							if((*it)->type() != CEntity::Bot)
							{
								(*it)->LastSentPing.push(make_pair(pingnb, currentTime));
//								nlinfo("*********** send the ping %u %"NL_I64"u", pingnb, currentTime);
							}

							CVector newPos = (*it)->LastSent2OthersPos + sendDPos; 
							(*it)->LastSent2OthersPos = newPos;
							(*it)->LastSent2MePos = newPos;
						}
					}
				}
			}
			
			CNetwork::getInstance().send(msgout);
			UpdatePacketSize = msgout.length();
			pingnb++;
		}
		else
		{
			
			TTime currentTime = CTime::getLocalTime();
			
			for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
			{
				if((*it)->type() != CEntity::Bot)
				{
					CNetMessage msgout(CNetMessage::UpdateOne);
					const dReal *pos = dBodyGetPosition((*it)->Body);
					
					(*it)->Pos.x = (float)pos[0];
					(*it)->Pos.y = (float)pos[1]; 
					(*it)->Pos.z = (float)pos[2];
					
					uint8 eid = (*it)->id();
					uint16 ping = (*it)->Ping.getSmoothValue();
					//msgout.serial(eid);
					
					CVector dpos = (*it)->Pos - (*it)->LastSent2MePos;
					
					uint8 sx;
					uint8 dx;
					packBit32 pb32;
					CVector sendDPos;

					sendDPos.x = computeOut8_8fp(dpos.x,dx,sx);
					pb32.packBits(dx,4);
					pb32.packBits(sx,6);
					sendDPos.y = computeOut8_8fp(dpos.y,dx,sx);
					pb32.packBits(dx,4);
					pb32.packBits(sx,6);
					sendDPos.z = computeOut8_8fp(dpos.z,dx,sx);
					pb32.packBits(dx,4);
					pb32.packBits(sx,6);

					msgout.serial(pb32.bits);
					
					CVector newPos = (*it)->LastSent2MePos + sendDPos; 
					
					CNetwork::getInstance().send((*it)->id(),msgout,true);
					
					(*it)->LastSent2MePos = newPos;
				}

			}
		}
	}
	updateCount++;
}

void CNetwork::sleep(TTime timeout)
{
	H_AUTO(CNetworkSleep);

	TTime t0 = CTime::getLocalTime ();

	if (timeout > 0)
	{
		if (NextUpdateTime == 0)
		{
			NextUpdateTime = t0 + timeout;
		}
		else
		{
			TTime err = t0 - NextUpdateTime;
			NextUpdateTime += timeout;
			
			// if we are too late, resync to the next value
			while (err > timeout)
			{
				err -= timeout;
				NextUpdateTime += timeout;
			}
			
			timeout -= err;
			if (timeout < 0) timeout = 0;
		}
	}

	while (true)
	{
		BufServer->update();
		while(BufServer->dataAvailable())
		{
			TSockId sockid;
			CNetMessage msg(CNetMessage::Unknown, true);
			BufServer->receive(msg, &sockid);
			
			try
			{
				uint32 nbs = 0;
				msg.serial(nbs);
				
				uint8 t = 0;
				msg.serial(t);
				msg.type((CNetMessage::TType)t);
				
				{
					for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
					{
						nlassert(*it);
						CClient *c = (CClient *)(*it);
						
						if((*it)->type() != CEntity::Client)
							continue;
						
						nlassert(c->sock());
						
						if(c->sock() != sockid)
							continue;
						
						CEntityManager::EntityConstIt itNext = it;
						itNext++;
						uint8 eid = c->id();
						netCallbacksHandler(c, msg);
						bool currentEntityPresent = CEntityManager::getInstance().getById(eid)!=NULL;
						if(!currentEntityPresent)
						{
							it = itNext;
							if(it==CEntityManager::getInstance().entities().end())
								break;
						}	
					}
				}
				
				//TODO SKEET : check why we cannot let only main thread loop do the flush job ?
				//CEntityManager::getInstance().flushAddRemoveList();
			}
			catch(Exception &e)
			{
				nlwarning("Malformed Message type '%hu' : %s", (uint16)msg.type(), e.what());
			}
		}
		// If it's the end, don't nlSleep()
		TTime remainingTime = t0 + timeout - CTime::getLocalTime();
		if ( remainingTime <= 0 )
			break;

#ifdef NL_OS_UNIX
		// Sleep until the time expires or we receive a message
		H_BEFORE(UserSleep);
		nlSleep(1);
/*		fd_set readers;
		timeval tv;
		FD_ZERO( &readers );
		FD_SET( BufServer->dataAvailablePipeReadHandle(), &readers );
		tv.tv_sec = 0;
		tv.tv_usec = remainingTime * 1000;
		int res = ::select( BufServer->dataAvailablePipeReadHandle()+1, &readers, NULL, NULL, &tv );
		if ( res == -1 )
			nlerror( "LNETL1: Select failed in sleepUntilDataAvailable (code %u)", CSock::getLastError() );
*/		H_AFTER(UserSleep);
#else
		// Enable windows multithreading before rescanning all connections
		H_TIME(UserSleep, nlSleep(1);); // 0 (yield) would be too harmful to other applications
#endif
	
	}
}

void CNetwork::release()
{
//	if(fp)
//		fclose(fp);
	if(BufServer)
	{
		delete BufServer;
		BufServer = 0;
	}
}


void CNetwork::send(uint8 eid, CNetMessage &msg, bool checkReady)
{
	for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
	{
		if((*it)->id() == eid)
		{
			if((*it)->type() != CEntity::Client)
				nlwarning("(*it)->type() != CEntity::Client (%s)",(*it)->name().c_str());
			//nlassert((*it)->type() == CEntity::Client);
			CClient *c = (CClient *)(*it);
			if(!checkReady || c->networkReady())
			{
				bool sok = msg.send(BufServer, c->sock());
				if(!sok)
					CEntityManager::getInstance().remove(c->id());
			}
		}
	}
}

void CNetwork::send(CNetMessage &msg)
{
	for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
	{
		if((*it)->type() == CEntity::Client)
		{
			CClient *c = (CClient *)(*it);
			if(c->networkReady())
			{
				bool sok = msg.send(BufServer, c->sock());
				if(!sok)
					CEntityManager::getInstance().remove(c->id());
			}
		}
	}
}

void CNetwork::sendAllExcept(uint8 eid, CNetMessage &msg)
{
	for(CEntityManager::EntityConstIt it = CEntityManager::getInstance().entities().begin(); it != CEntityManager::getInstance().entities().end(); it++)
	{
		if((*it)->id() != eid && (*it)->type() == CEntity::Client)
		{
			CClient *c = (CClient *)(*it);
			if(c->networkReady())
			{
				bool sok = msg.send(BufServer, c->sock());
				if(!sok)
					CEntityManager::getInstance().remove(c->id());
			}
		}
	}
}

void CNetwork::sendChat(const string &msg, bool forward)
{
	CNetMessage msgout(CNetMessage::Chat);
	msgout.serial(const_cast<string&>(msg));
	CNetwork::getInstance().send(msgout);
	if(forward)
		forwardToPublicChat(msg);
}

void CNetwork::sendChat(uint8 eid,const string &msg, bool forward)
{
	CNetMessage msgout(CNetMessage::Chat);
	msgout.serial(const_cast<string&>(msg));
	CNetwork::getInstance().send(eid,msgout);
	if(forward)
		forwardToPublicChat(msg);
}

void CNetwork::forwardToPublicChat(const std::string msg)
{
	if(!ChatSock.connected())
		return;
	sendToPublicChat(msg);
	tellToPublicChat(msg);
}

void CNetwork::tellToPublicChat(const std::string msg)
{
	if(!ChatSock.connected())
		return;
	for(uint i = 0; i < IService::getInstance()->ConfigFile.getVar("publicChatForwardTo").size(); i++)
	{
		string nickName = IService::getInstance()->ConfigFile.getVar("publicChatForwardTo").asString(i);
		sendToPublicChat("tell " + nickName + " " + msg);
	}
}

void CNetwork::sendToPublicChat(const std::string msg)
{
	if(!ChatSock.connected())
		return;
	string msgline = msg + "\n";
	uint32 len = msgline.size();
	ChatSock.send((const uint8 *)msgline.c_str(),len);
}
