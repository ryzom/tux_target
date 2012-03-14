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

#ifndef MTPT_SESSION_STATE
#define MTPT_SESSION_STATE


//
// Class
//

class CSessionState
{
public:

	virtual void enter() { }		// called each time the state machine goes to this state
	virtual void update() { } 		// called each loop when the state machine is in this state
	virtual void leave() { }		// called each time the state machine leaves to this state

	virtual std::string name() const = 0;
	virtual std::string shortName() const = 0;

protected:

	void changeState(CSessionState &ns);

private:

};

#endif
