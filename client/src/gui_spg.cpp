/* Copyright, 2010 Tux Target
 * Copyright, 2008 Vialek (Vianney Lecroart - gpl@vialek.com)
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

#include "gui_spg.h"


//
// Functions
//

bool guiSPGCounter::release() 
{ 
	if (--refCounter <= 0) 
	{
		guiSPGManager::instance().remove(p);
		delete this;
		return true;
	}
	return false;
};


guiSPGManager *guiSPGManager::_instance = 0;

guiSPGManager &guiSPGManager::instance()
{
	if(_instance==0)
		_instance = new guiSPGManager;
	return *_instance;
}

guiSPGCounter *guiSPGManager::get(void *p)
{
	pointer2Counter::iterator it;
	it = _pointer2Counter.find(p);
	if(it==_pointer2Counter.end())	
	{
		guiSPGCounter *res = new guiSPGCounter(p);
		add(p,res);
		return res;
	}
	return (*it).second;
}

void guiSPGManager::add(void *p,guiSPGCounter *counter)
{
	_pointer2Counter.insert(pointer2Counter::value_type(p,counter));	
}

void guiSPGManager::remove(void *p)
{
	pointer2Counter::iterator it;
	it = _pointer2Counter.find(p);
	if(it==_pointer2Counter.end())	
		return ;

	_pointer2Counter.erase(it);
}



