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

#include "3d_task.h"
#include "gui_xml.h"
#include "gui_text.h"
#include "time_task.h"
#include "mtp_target.h"
#include "font_manager.h"
#include "gui_multiline_text.h"
#include "gui_stretched_quad.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

int CGuiText::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession, name().c_str());
	return 1;
}

int CGuiText::getString(lua_State *luaSession)
{
	lua_pushstring(luaSession, Text.toUtf8().c_str());
	return 1;
}

int CGuiText::setString(lua_State *luaSession)
{
	size_t len;
	const char *cnewText= luaL_checklstring(luaSession, 1, &len);
	setText(ucstring::makeFromUtf8(cnewText));
	return 0;
}

void CGuiText::setText(const ucstring &text)
{
	if(!isEntry() && isI18N()) Text = convertVariableString(text);
	else Text = text;

	TextSize = CGuiMultilineText::size(false, CFontManager::instance().guiFontSize(), Text);
}

template<class OutIt> void split(const ucstring &s, ucchar sep, OutIt dest)
{
	ucstring::size_type left = 0;
	ucstring::size_type right = left;
	while (left <= s.size())
	{
		right = left;
		for(; right != s.size() && s[right] != sep; right++)
			;
		*dest = s.substr (left, right-left);
		++dest;
		if(right == s.size())
			break;
		left = right + 1;
	}
}

void CGuiTextManager::init()
{
	string res;
	res = CPath::lookup("cursor.tga");
	CursorTexture = C3DTask::instance().driver().createTextureFile(res);
	nlassert(CursorTexture);
	CursorTexture->setWrapS(UTexture::Clamp);
	CursorTexture->setWrapT(UTexture::Clamp);

	CursorMaterial = C3DTask::instance().createMaterial();
	CursorMaterial.setTexture(CursorTexture);
	CursorMaterial.setBlend(true);
	CursorMaterial.setZFunc(UMaterial::always);
	CursorMaterial.setDoubleSided();

	res = CPath::lookup("entry.tga");
	EntryTexture = C3DTask::instance().driver().createTextureFile(res);
	nlassert(EntryTexture);
	EntryTexture->setWrapS(UTexture::Clamp);
	EntryTexture->setWrapT(UTexture::Clamp);

	EntryMaterial = C3DTask::instance().createMaterial();
	EntryMaterial.setTexture(EntryTexture);
	EntryMaterial.setBlend(true);
	EntryMaterial.setZFunc(UMaterial::always);
	EntryMaterial.setDoubleSided();

	CGuiText::xmlRegister();
	CGuiTextPercent::xmlRegister();
}

void CGuiTextManager::render()
{
}

void CGuiTextManager::release()
{
}

UMaterial CGuiTextManager::cursorMaterial()
{
	return CursorMaterial;
}

UMaterial CGuiTextManager::entryMaterial()
{
	return EntryMaterial;
}

//
//
//

TTime CGuiTextCursor::CursorBlinkTime = 0;

void CGuiTextCursor::reset()
{
	CursorBlinkTime = CTime::getLocalTime();
}

void CGuiTextCursor::_render(const CVector &position,int height)
{
	H_AUTO2;
	
	TTime ct = CTime::getLocalTime();
	if(ct - CursorBlinkTime < 500)
	{
		CGuiStretchedQuad quad;
		quad.stretched(false);
		quad.material(CGuiTextManager::instance().cursorMaterial());
		quad.size(CVector(2,(float)height,0));
		quad.position(position);
		quad.render();
	}
	if(ct - CursorBlinkTime > 1000)
	{
		CursorBlinkTime = ct;
	}
}


//
//
//

void CGuiText::init(const ucstring &text)
{
	CursorIndex = 0;	// must be init here because cursorIndex() make a if() with this value that is not init the first time
	cursorIndex(0);
	IsEditable  = false;
	IsEntry     = false;
	IsPassword  = false;
	IsMultiline = true;
	IsI18N      = true;
	setText(text);
}

CGuiText::CGuiText(const ucstring &text, bool i18n)
{
	init(ucstring(""));
	IsI18N = i18n;
	setText(text);
}

CGuiText::CGuiText()
{
	init(ucstring(""));
}

CGuiText::~CGuiText()
{
}

void CGuiText::insert(ucchar c)
{
	ucstring insertedString;
	insertedString += c;
	setText(Text.substr(0,cursorIndex()) + insertedString + Text.substr(cursorIndex(), Text.size()));
	cursorIndex(cursorIndex()+1);
	CGuiTextCursor::reset();
}

static ucstring strToPassword(const ucstring &str, bool active)
{
	if(!active) return str;

	ucstring res;
	for(uint i = 0; i < str.size(); i++)
	{
		if(str[i] == '\n')
			res += '\n';
		else
			res += '*';
	}
	return res;
}

static ucstring strToMultiline(const ucstring &str, bool active)
{
	if(active) return str;

	ucstring res;
	for(uint i = 0; i < str.size(); i++)
	{
		if(str[i] != '\n')
			res += str[i];
	}
	return res;
}

void CGuiText::text(const ucstring &t)
{
	setText(t);
	cursorIndex(t.size());
}


void CGuiText::_render(const CVector &pos, CVector &maxSize)
{
	H_AUTO2;
	CVector globalPos = globalPosition(pos,maxSize);
	CVector cursorPos(0,0,0);

	CVector expSize = expandSize(maxSize);

	if(isEntry())
	{
		CGuiStretchedQuad quad;
		quad.material(CGuiTextManager::instance().entryMaterial());
		quad.size(expSize);
		quad.position(globalPosition(pos,maxSize));
		quad.render();
	}

	if(isEntry())
		globalPos += CVector(2,2,0);

	ucstring text;
	text = strToMultiline(Text, isMultiline());
	CGuiMultilineText::print(globalPos.x , globalPos.y, cursorIndex(), cursorPos, strToPassword(text, isPassword()));

	if(focused() && isEntry())
	{
		CGuiTextCursor::_render(cursorPos, CFontManager::instance().guiFontSize());
		if(isEditable())
		{
			ucstring res = C3DTask::instance().kbGetString();
			for (uint32 i = 0; i < res.size() ; i++)
			{
				if (res[i] == 8)
				{ // Backspace
					if (text.length() > 0 && cursorIndex() > 0)
					{
						setText(text.substr(0, cursorIndex()-1) + text.substr(cursorIndex(), text.size()));
						cursorIndex(cursorIndex()-1);
					}
					continue;
				}
				else if(res[i] == '\r')
				{
					if(isMultiline())
						insert('\n');
				}
				else
				{
					insert(res[i]);
				}
			}

			if (C3DTask::instance().kbPressed(KeyDELETE))
			{
				if (text.length() > 0 && cursorIndex() < text.size())
				{
					setText(text.substr(0, cursorIndex()) + text.substr(cursorIndex()+1, text.size()));
				}
			}

#ifdef NL_OS_WINDOWS
			// Insert the clipboard
			if (C3DTask::instance().kbDown(KeySHIFT) && C3DTask::instance().kbPressed(KeyINSERT))
			{
				if (OpenClipboard(NULL))
				{
					HANDLE hObj=GetClipboardData(CF_UNICODETEXT);
					if (hObj)
					{
						wchar_t *sString=(wchar_t*)GlobalLock(hObj);
						if(sString != NULL)
						{
							for(unsigned int i = 0; i < wcslen(sString); i++)
							{
								ucchar ch = ucchar(sString[i]);
								if(ch == '\t') ch = ' ';
								else if(ch == '\r') continue;
								else if(ch == '\n') break;
								insert(ch);
							}
							GlobalUnlock (hObj);
						}
					}
					CloseClipboard ();
				}
			}
#endif // NL_OS_WINDOWS

			if (C3DTask::instance().kbPressed(KeyUP))
				cursorUp();
			if (C3DTask::instance().kbPressed(KeyDOWN))
				cursorDown();
			if (C3DTask::instance().kbPressed(KeyLEFT))
				cursorLeft();
			if (C3DTask::instance().kbPressed(KeyRIGHT))
				cursorRight();
			if (C3DTask::instance().kbPressed(KeyHOME))
				cursorHome();
			if (C3DTask::instance().kbPressed(KeyEND))
				cursorEnd();
		}
	}

	maxSize = expSize;
}


void CGuiText::cursorHome()
{
	vector<ucstring> vstr;
	split(Text, '\n', back_inserter(vstr));

	uint pos = 0;
	uint column;
	uint line;
	uint home;
	for(uint j = 0; j < vstr.size(); j++)
	{
		home = pos;
		for(uint i = 0; i <= vstr[j].size(); i++, pos++)
		{
			if(pos == cursorIndex())
			{
				column = i;
				line = j;
				cursorIndex(home);
				return;
			}
		}
	}
}

void CGuiText::cursorEnd()
{
	vector<ucstring> vstr;
	split(Text, '\n', back_inserter(vstr));

	uint pos = 0;
	uint column;
	uint line;
	uint home;
	for(uint j=0;j<vstr.size();j++)
	{
		home = pos;
		for(uint i=0;i<=vstr[j].size();i++,pos++)
		{
			if( pos==cursorIndex() )
			{
				column = i;
				line = j;
				cursorIndex(home + vstr[j].size());
				return;
			}
		}
	}

}

void CGuiText::cursorUp()
{
	vector<ucstring> vstr;
	split(Text, '\n', back_inserter(vstr));

	uint pos = 0;
	uint column = 0;
	uint line = 0;
	for(uint j=0;j<vstr.size();j++)
	{
		for(uint i=0;i<=vstr[j].size();i++,pos++)
		{
			if( pos==cursorIndex() )
			{
				column = i;
				line = j;
			}
		}
	}

	pos=0;
	for(uint j=0;j<vstr.size();j++)
	{
		if( j==(line-1) )
		{
			if( column>vstr[j].size() )
				cursorIndex(pos + vstr[j].size());
			else
				cursorIndex(pos + column);
		}
		else
			pos += vstr[j].size()+1;
	}
}


void CGuiText::cursorDown()
{
	vector<ucstring> vstr;
	split(Text, '\n', back_inserter(vstr));

	uint pos = 0;
	uint column = 0;
	uint line = 0;
	for(uint j=0;j<vstr.size();j++)
	{
		for(uint i=0;i<=vstr[j].size();i++,pos++)
		{
			if( pos==cursorIndex() )
			{
				column = i;
				line = j;
			}
		}
	}

	pos=0;
	for(uint j=0;j<vstr.size();j++)
	{
		if( j==(line+1) )
		{
			if( column>vstr[j].size() )
				cursorIndex(pos + vstr[j].size());
			else
				cursorIndex(pos + column);
		}
		else
			pos += vstr[j].size()+1;
	}
}

void CGuiText::cursorLeft()
{
	if(cursorIndex()>0)
	{
		cursorIndex(cursorIndex()-1);
		CGuiTextCursor::reset();
	}
}

void CGuiText::cursorRight()
{
	cursorIndex(cursorIndex()+1);
	if(cursorIndex() > Text.size())
		cursorIndex(Text.size());
	else
		CGuiTextCursor::reset();
}

float CGuiText::_width()
{
	float w = TextSize.x;
	if(isEntry())
		return w + 4;
	else
		return w + 4;
}

float CGuiText::_height()
{
	float h = TextSize.y;
	if(isEntry())
		return h + 4;
	else
		return h;
}

void CGuiText::cursorIndex(uint cursorIndex)
{
	if(cursorIndex<0)
		cursorIndex = 0;
	if(CursorIndex != cursorIndex)
	{
		CursorIndex = cursorIndex;
		CGuiTextCursor::reset();
	}
}

uint CGuiText::cursorIndex()
{
	return CursorIndex;
}

void CGuiText::luaPush(lua_State *L)
{
	Lunar<CGuiText>::push(L, this);
}

void CGuiText::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiText", CGuiText::create);
}

CGuiObject *CGuiText::create()
{
	CGuiObject *res = new CGuiText;
	return res;
}
	
void CGuiText::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiObject::init(xml,node);

	bool b;
	if(xml->getBool(node,"isEntry",b))
		isEntry(b);
	if(xml->getBool(node,"isEditable",b))
		isEditable(b);
	if(xml->getBool(node,"isPassword",b))
		isPassword(b);
	if(xml->getBool(node,"isMultiline",b))
		isMultiline(b);
	if(xml->getBool(node,"isI18N",b))
		isI18N(b);

	string res;
	xml->getString(node,"string",res);
	setText(ucstring::makeFromUtf8(res));
}


//
//
//

CGuiTextPercent::CGuiTextPercent() : CGuiText()
{
	PtrValue = 0;
	isI18N(false);
}

CGuiTextPercent::~CGuiTextPercent()
{
}

void CGuiTextPercent::_render(const CVector &pos, CVector &maxSize)
{
	H_AUTO2;
	char ch[256];
	if(PtrValue)
		sprintf(ch, "%.0f%%", *PtrValue * 100);
	else
		sprintf(ch, "0%");
	setText(ucstring(ch));
	CGuiText::_render(pos, maxSize);
}

void CGuiTextPercent::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiTextPercent",CGuiTextPercent::create);
}

CGuiObject *CGuiTextPercent::create()
{
	CGuiObject *res = new CGuiTextPercent;
	return res;
}

void CGuiTextPercent::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiText::init(xml,node);
}
