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
// This is the main class that manages all other classes
//

#ifndef MTPT_GUI_OBJECT_H
#define MTPT_GUI_OBJECT_H


//
// Includes
//

#include <nel/misc/singleton.h>

#include "gui_mouse_listener.h"
#include "gui_event_behaviour.h"
#include "gui_spg.h"
#include <libxml/parser.h>
#include "../../common/lua_utility.h"

NLMISC::CVector CVectorMax(const NLMISC::CVector &a, const NLMISC::CVector &b);
class CGuiXml;	

//
// Classes
//
class CGuiObject
{
public:

	enum TGuiAlignment {
			eAlignNone             = 0x0,
			eAlignCenterVertical   = 0x1,
			eAlignCenterHorizontal = 0x2,
			eAlignUp               = 0x4,
			eAlignBottom           = 0x8,
			eAlignLeft             = 0x10,
			eAlignRight            = 0x20,
			eAlignExpandVertical   = 0x40,
			eAlignExpandHorizontal = 0x80,
	};

	CGuiObject();
	CGuiObject(lua_State *luaSession) {};
	virtual ~CGuiObject();
	
	virtual void init();
	virtual void update();
	void render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual void release();
	
	//virtual bool mouseIsIn(NLMISC::CVector pos,NLMISC::CVector maxSize);
	bool isIn(const NLMISC::CVector &point, const NLMISC::CVector &startPos, const NLMISC::CVector &size);
	static float ToProportionalX(float x);
	static float ToProportionalY(float y);
	virtual float width();
	virtual float height();
	virtual NLMISC::CVector position();
	virtual NLMISC::CVector globalPosition(const NLMISC::CVector &pos, const NLMISC::CVector &maxSize);
	virtual NLMISC::CVector expandSize(const NLMISC::CVector &maxSize);
	virtual NLMISC::CVector size();
	virtual NLMISC::CVector renderedSize();
	virtual NLMISC::CVector renderedPos();
	virtual void  position(const NLMISC::CVector &position);
	
	virtual TGuiAlignment alignment();
	virtual void alignment(int alignment);
	NLMISC::CVector offset(const NLMISC::CVector &maxSize);

	NL3D::UMaterial LoadBitmap(const std::string &filename);
		
	//CVector globalPosition()

	bool focused();
	float minWidth();
	float minHeight();
	void minWidth(float minWidth);
	void minHeight(float minHeight);

	NLMISC::CVector minSize();
	void minSize(const NLMISC::CVector &minSize);
	
	std::string name();
	std::string getClassName();
	void setClassName(const std::string);

	static CGuiObject *XmlCreateFromNode(CGuiXml *xml,xmlNodePtr node);
	virtual void init(CGuiXml *xml,xmlNodePtr node);
		
	virtual void luaPush(lua_State *L);
	int getName(lua_State *luaSession);
	static const char className[];	
	static Lunar<CGuiObject>::RegType methods[];	
	
friend class CGuiXmlManager;	
friend class CGuiXml;	
protected:
	CGuiXml *_xml;
	std::string _name;
	std::string _className;
	virtual float _width() {return 0;};
	virtual float _height() {return 0;};
	virtual void _preRender(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual void _postRender(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	void _checkFocus(const NLMISC::CVector &pos, const NLMISC::CVector &maxSize);
private:
	TGuiAlignment _alignment;
	NLMISC::CVector _position;	
	NLMISC::CVector _minSize;	
	NLMISC::CVector _renderedSize;	
	NLMISC::CVector _renderedPos;	
};

typedef CGuiObject * (*CreateObjectCB) ();
typedef std::map<std::string,CreateObjectCB> string2CreateFunction;


class CGuiObjectManager : public NLMISC::CSingleton<CGuiObjectManager>
{
public:
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	void registerClass(const std::string &className,CreateObjectCB cb);
	CGuiObject *create(const std::string &className);
		
	CGuiMouseListener &mouseListener();
	void focus(CGuiObject *object);
	CGuiObject *focus();

	std::list<CGuiObject *> objects;
protected:
private:
	guiSPG<CGuiMouseListener> _mouseListener;
	CGuiObject *_focus;
	string2CreateFunction _createFunctionMap;
};


#endif
