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

#ifndef MT_EDITOR_TASK_H
#define MT_EDITOR_TASK_H


//
// Includes
//

#include "gui.h"
#include "module.h"


//
// Classes
//

class CEditorTask : public NLMISC::CSingleton<CEditorTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual string name() const { return "CEditorTask"; }

	void enable(bool e);
	bool enable();
	void reset();
	
	
	CEditableElementCommon *selectedElement() {return _selectedElement;}
	
private:

	void _mouseSelectModule();
		
	bool _enable;
	guiSPG<CGuiFrame> _testFrame;
	CEditableElementCommon *_selectedElement;
	float	_mouseX;
	float	_mouseY;
	double  _lastUpdateTime;
};

#endif
