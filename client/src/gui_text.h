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

#ifndef MT_GUI_TEXT_H
#define MT_GUI_TEXT_H


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
	static void _render(const NLMISC::CVector &position, int height);
	static void reset();
private:
	static NLMISC::TTime CursorBlinkTime;
};

class CGuiText : public CGuiObject
{
public:
	CGuiText(const ucstring &text, bool i18n = true);
	CGuiText();
	virtual ~CGuiText();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	virtual float _width();
	virtual float _height();

	void isEditable(bool isEditable) { IsEditable = isEditable; }
	bool isEditable() const { return IsEditable; }
	
	void isEntry(bool isEntry) { IsEntry = isEntry; }
	bool isEntry() const { return IsEntry; }
	
	void isPassword(bool isPassword) { IsPassword = isPassword; }
	bool isPassword() const { return IsPassword; }
	
	void isMultiline(bool isMultiline) { IsMultiline = isMultiline; }
	bool isMultiline() { return IsMultiline; }

	void isI18N(bool isI18N) { IsI18N = isI18N; }
	bool isI18N() const { return IsI18N; }
	
	void cursorIndex(uint cursorIndex);
	uint cursorIndex();
	
	const ucstring &text() { return Text; }
	void text(const ucstring &t);

	void insert(ucchar c);

	void cursorUp();
	void cursorDown();
	void cursorLeft();
	void cursorRight();
	void cursorHome();
	void cursorEnd();

	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);

	virtual void luaPush(lua_State *L);

	LUA_BEGIN(CGuiText)
		bind_method(CGuiText, getName),	
		bind_method(CGuiText, getString),	
		bind_method(CGuiText, setString),	
	LUA_END

	int getName(lua_State *luaSession);
	int getString(lua_State *luaSession);
	int setString(lua_State *luaSession);

	void setText(const ucstring &text);

protected:

	ucstring Text;

private:

	void init(const ucstring &text);

	// cache system to speed up text rendering
	NLMISC::CVector TextSize;

	uint32 CursorIndex;
	bool IsEditable;
	bool IsEntry;
	bool IsPassword;
	bool IsMultiline;
	bool IsI18N;
};

class CGuiTextPercent : public CGuiText
{
public:

	CGuiTextPercent();
	virtual ~CGuiTextPercent();
	
	virtual void _render(const NLMISC::CVector &pos, NLMISC::CVector &maxSize);
	void ptrValue(float *ptrValue) { PtrValue = ptrValue; }

	static void xmlRegister();
	static CGuiObject *create();
	virtual void init(CGuiXml *xml,xmlNodePtr node);

private:
	float *PtrValue;
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
	NL3D::UTextureFile	*CursorTexture;
	NL3D::UMaterial CursorMaterial;
	NL3D::UTextureFile	*EntryTexture;
	NL3D::UMaterial EntryMaterial;
private:
	
};

#endif
