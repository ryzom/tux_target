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

#ifndef MTPT_GUI_TEXT_H
#define MTPT_GUI_TEXT_H


//
// Includes
//

#include "gui_object.h"



//
// Classes
//

class CGuiTextCursor
{
public:
	static void Render(const NLMISC::CVector &position, int height);
	static void Reset();
private:
	static int _FrameCount;
};



class CGuiText : public CGuiObject
{
	public:
	CGuiText(lua_State *luaSession) {};
	CGuiText(const std::string &text);
	CGuiText();
	virtual ~CGuiText();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual float _width();
	virtual float _height();

	void isEditable(bool isEditable);
	bool isEditable();
	
	void isEntry(bool isEntry);
	bool isEntry();
	
	void isPassword(bool isPassword);
	bool isPassword();
	
	void isMultiline(bool isMultiline);
	bool isMultiline();
	
	void cursorIndex(int cursorIndex);
	uint cursorIndex();
	
	void insert(char c);

	void cursorUp();
	void cursorDown();
	void cursorLeft();
	void cursorRight();
	void cursorHome();
	void cursorEnd();
	
	std::string text;

	static void XmlRegister();
	static CGuiObject *Create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);

	virtual void luaPush(lua_State *L);
	static const char className[];	
	static Lunar<CGuiText>::RegType methods[];	
	int getName(lua_State *luaSession);
	int getString(lua_State *luaSession);
	int setString(lua_State *luaSession);
private:
	void _init(const std::string &text);
	uint _cursorIndex;
	bool _isEditable;
	bool _isEntry;
	bool _isPassword;
	bool _isMultiline;
};


class CGuiTextPercent : public CGuiText
{
public:
	CGuiTextPercent();
	virtual ~CGuiTextPercent();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	void ptrValue(float *ptrValue);

	static void XmlRegister();
	static CGuiObject *Create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);
private:
	float *_ptrValue;
};



	
class CGuiTextManager : public NLMISC::CSingleton<CGuiTextManager>
{
public:
	virtual void init();
	virtual void update() { }
	virtual void render();
	virtual void release();

	NL3D::UMaterial cursorMaterial();
	NL3D::UMaterial entryMaterial();
	
protected:
	NL3D::UTextureFile	*_cursorTexture;
	NL3D::UMaterial _cursorMaterial;
	NL3D::UTextureFile	*_entryTexture;
	NL3D::UMaterial _entryMaterial;
private:
	
};

#endif
