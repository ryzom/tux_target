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

#ifndef MT_GUI_OBJECT_H
#define MT_GUI_OBJECT_H


//
// Includes
//

#include <libxml/parser.h>

#include <nel/misc/singleton.h>

#include "gui_spg.h"
#include "level_manager.h"
#include "gui_mouse_listener.h"
#include "gui_event_behaviour.h"
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

	NL3D::UMaterial LoadBitmap(const string &filename);
		
	//CVector globalPosition()

	void visible(bool vis) { Visible = vis; }
	bool visible() const { return Visible; }

	bool focused();
	float minWidth();
	float minHeight();
	void minWidth(float minWidth);
	void minHeight(float minHeight);

	NLMISC::CVector minSize();
	void minSize(const NLMISC::CVector &minSize);
	
	string name();
	string getClassName();
	void setClassName(const string &cname);

	static CGuiObject *XmlCreateFromNode(CGuiXml *xml,xmlNodePtr node);
	virtual void init(CGuiXml *xml,xmlNodePtr node);
		
	virtual void luaPush(lua_State *L);
	int getName(lua_State *luaSession);

	LUA_BEGIN(CGuiObject)
		LUA_BIND(CGuiObject, getName),	
	LUA_END

friend class CGuiXmlManager;	
friend class CGuiXml;	

protected:

	CGuiXml *_xml;
	string _name;
	string _className;
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
	bool			Visible;
};

typedef CGuiObject * (*CreateObjectCB) ();
typedef map<string,CreateObjectCB> string2CreateFunction;


class CGuiObjectManager : public NLMISC::CSingleton<CGuiObjectManager>
{
public:
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	void registerClass(const string &className,CreateObjectCB cb);
	CGuiObject *create(const string &className);

	CGuiMouseListener &mouseListener();
	void focus(CGuiObject *object);
	CGuiObject *focus();

	// object manipulation
	bool empty() const { return objects.empty(); }

	void clear() { backObjects.clear(); backObjectsChanged=true; }
	void addFrame(const std::string &frameName) { nlassert(Frames.find(frameName) != Frames.end()); backObjects.push_back(Frames[frameName]); backObjectsChanged=true; }
	void displayFrame(const std::string &frameName) { clear(); addFrame(frameName); }
	void removeFrame(const std::string &frameName);

	CGuiXml *loadFrame(const std::string &frameName, void (*uc)() = 0);
	void registerFrame(const std::string &frameName, CGuiObject *frame);

private:
	list<CGuiObject *> objects;
	guiSPG<CGuiMouseListener> _mouseListener;
	CGuiObject *_focus;
	string2CreateFunction _createFunctionMap;

	list<CGuiObject *> backObjects;
	bool backObjectsChanged;

	CHashMap<std::string, guiSPG<CGuiObject> > Frames;

	friend class CGuiObject;
};


#endif
