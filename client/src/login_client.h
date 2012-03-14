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

#ifndef LOGIN_CLIENT_H
#define LOGIN_CLIENT_H


//
// Includes
//

#include <nel/misc/types_nl.h>

#include <string>
#include <vector>

#include <nel/net/udp_sock.h>
#include <nel/net/login_cookie.h>
#include <nel/net/callback_client.h>


//
// Classes
//

class CLoginClientMtp
{
public:

	struct CShardEntry
	{
		CShardEntry() { ShardNbPlayers = 0; }
		CShardEntry(const std::string &name, uint8 nbp, uint32 sid) : ShardName(name), ShardNbPlayers(nbp), ShardId(sid)
		{
		}
		std::string ShardName;
		uint8		ShardNbPlayers;
		uint32		ShardId;
	};

	typedef std::vector<CShardEntry> TShardList;


	/** Set the graphics informations (card name, etc...) that will be send to the Login Service
	 *	ex: CLoginClient::setInformations (CNELU::Driver->getVideocardInformation ());
	 */
	//static void setInformations (std::string gfxInfos) { _GfxInfos = gfxInfos; }


	/** Tries to connect to the authentication server.
	 * Generates a ESocketConnectionFailed if it can't establish the connection.
	 */
	/** Tries to login with login and password.
	 * If the authentication is ok, the function return an empty string else it returns the reason of the failure.
	 */
	static std::string authenticate (const std::string &loginServiceAddr, const std::string &login, const std::string &password, uint32 clientVersion);


	/** Try to connect to the shard and return a TCP connection to the shard.
	 */
	static std::string connectToShard (uint32 shardListIndex, NLNET::CInetAddress &ip, std::string &cookie);
	
	static uint32 shardIdToIndex (uint32 shardId);

	static TShardList ShardList;

private:

	static std::string confirmConnection (uint32 shardListIndex);

	static std::string _GfxInfos;

	static NLNET::CCallbackClient *_CallbackClient;

};

#endif // LOGIN_CLIENT_H

/* End of login_client.h */
