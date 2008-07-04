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



