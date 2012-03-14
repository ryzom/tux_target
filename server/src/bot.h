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


#ifndef MTPT_BOT
#define MTPT_BOT


//
// Includes
//

#include "entity.h"


//
// Classes
//

class CBot : public CEntity
{
public:

	CBot(uint8 eid, const std::string &name, bool isAutomaticBot);
	virtual ~CBot();

	void	loadBotReplay();
	void	getBotDelta(NLMISC::CVector &delta);
	
	virtual void update();

	virtual void display(NLMISC::CLog &log = *NLMISC::InfoLog);
	
	// accessors

	bool	isAutomaticBot() const { return IsAutomaticBot; }	
	virtual bool	forceReceived() {return true;}
	
private:

	bool	IsAutomaticBot;

	std::string	findNewBotName(const std::string &name);
	
	// commands contains the road map for this bot

	class CCommand
	{
	public:
		
		CCommand(float time, const NLMISC::CVector &force, const NLMISC::CVector &pos) :
		  Time(time), Type(Position), Force(force), Pos(pos)
		{
		}
		
		CCommand(float time, const NLMISC::CVector &pos) :
		Time(time), Type(OpenClose), Force(NLMISC::CVector::Null), Pos(pos)
		{
		}
		
		enum TCommand { Position, OpenClose };
		
		float			Time;
		TCommand		Type;
		NLMISC::CVector Force;
		NLMISC::CVector Pos;
	};

	std::deque<CCommand> Commands;
};

#endif
