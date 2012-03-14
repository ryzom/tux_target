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

#ifndef MTPT_WAITING_CLIENTS_SESSION_STATE
#define MTPT_WAITING_CLIENTS_SESSION_STATE


//
// Include
//

#include <nel/misc/singleton.h>

#include "session_state.h"


//
// Class
//

class CWaitingClientsSessionState : public CSessionState, public NLMISC::CSingleton<CWaitingClientsSessionState>
{
public:
	
	virtual void enter() {	firstUpdate = true; }
	virtual void update();
	virtual std::string name() const { return "Waiting clients"; }
	virtual std::string shortName() const { return "WC"; }
	void restart() { _restart = true; }

private:
	CWaitingClientsSessionState() { _restart = false; }
	friend class NLMISC::CSingleton<CWaitingClientsSessionState>;
	bool _restart;
	bool firstUpdate;
};

#endif
