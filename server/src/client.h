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

#ifndef MTPT_CLIENT
#define MTPT_CLIENT


//
// Includes
//

#include "entity.h"


//
// Functions
//

class CClient : public CEntity
{
public:

	CClient(uint8 id, NLNET::TSockId sock);

	virtual ~CClient();

	// update the new force vector sent by the client
	virtual void setForce(const NLMISC::CVector &clientForce);

	virtual bool openClose();
	
	virtual void display(NLMISC::CLog &log = *NLMISC::InfoLog);
		
	void networkReady(bool b) { NetworkReady = b; }
	bool networkReady() const { return NetworkReady; }

	// accessors

	NLNET::TSockId			sock() const { return Sock; }

	virtual bool			forceReceived();

	sint32 uid() const { return UId; }
	void uid(sint32 uid) { UId = uid; }

public:

	// ugly public variables

	std::string			 Cookie;
	FILE				*ReplayFile;
	std::string			 ReplayFilename;

private:

	NLNET::TSockId		Sock;

	// we send update information about this entity
	bool NetworkReady;

	// uniq id for all shards, gived by the login service
	sint32	UId;
};

#endif
