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

#ifndef MT_LOGIN_CLIENT_H
#define MT_LOGIN_CLIENT_H


//
// Includes
//

#include <nel/misc/types_nl.h>

#include <nel/net/udp_sock.h>
#include <nel/net/login_cookie.h>
#include <nel/net/callback_client.h>


//
// Variables
//

extern std::map<std::string, float> MyRecords;


//
// Classes
//

class CLoginClientMtp
{
public:

	struct CShardEntry
	{
		CShardEntry() { ShardNbPlayers = 0; }
		CShardEntry(const ucstring &name, uint8 nbp, uint32 sid) : ShardName(name), ShardNbPlayers(nbp), ShardId(sid)
		{
		}
		ucstring	ShardName;
		uint8		ShardNbPlayers;
		uint32		ShardId;
	};

	typedef vector<CShardEntry> TShardList;

	/** Tries to login with login and password. the password is md5 crypted (that's why it s a string and not a ucstring)
	 * If the authentication is ok, the function return an empty string else it returns the reason of the failure.
	 */
	static string authenticate (const string &loginServiceAddr, const ucstring &login, const string &password);

	/** Try to connect to the shard and return a TCP connection to the shard.
	 */
	static string connectToShard (uint32 shardListIndex, NLNET::CInetAddress &ip, string &cookie);

	static uint32 shardIdToIndex (uint32 shardId);

	static const CShardEntry &selectedShard() { nlassert(SelectedShardIndex>=0 && SelectedShardIndex<ShardList.size()); return ShardList[SelectedShardIndex]; }

	static TShardList ShardList;

private:

	static string confirmConnection (uint32 shardListIndex);

	static NLNET::CCallbackClient *_CallbackClient;
	
	static uint32 SelectedShardIndex;
};

#endif
