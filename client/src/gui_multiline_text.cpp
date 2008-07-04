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

#include <nel/3d/u_material.h>

#include "3d_task.h"
#include "time_task.h"
#include "gui_object.h"
#include "font_manager.h"
#include "gui_multiline_text.h"


//
// Namespaces
//

using namespace NL3D;


//
// Variables
//

static const float MultilineStringYSpace = 3;


//
// Functions
//
	
template<class OutIt> void split(const ucstring& s, char sep, OutIt dest) 
{
	ucstring::size_type left = 0;
	ucstring::size_type right = left;
	while( left <= s.size())
	{
		right=left;
		for(;right != s.size() && s[right]!=sep;right++);
		*dest = s.substr( left, right-left );
		++dest;
		if(right==s.size()) break;
		left = right+1;
	}
}

CGuiMultilineText::CGuiMultilineText()
{
}

CGuiMultilineText::~CGuiMultilineText()
{
}

void CGuiMultilineText::printf(float x, float y, int cursorIndex, CVector &cursorPos, const char *format ...)
{
	string str;
	NLMISC_CONVERT_VARGS (str, format, 256);

	print(x,y,cursorIndex,cursorPos,ucstring(str));
}

void CGuiMultilineText::print(float x, float y, int cursorIndex, CVector &cursorPos, const ucstring &str)
{
	H_AUTO2;
	if(str.size()==0)
	{
		cursorPos.x = x;
		cursorPos.y = y;
		return;
	}

	static float StringHeight = 0.0f;
	static float StringLine = 0.0f;

	if(StringHeight == 0.0f)
	{
		UTextContext::CStringInfo defaultStringInfo = CFontManager::instance().textContext(CFontManager::TCGui).getStringInfo(ucstring("Yg"));
		StringHeight = defaultStringInfo.StringHeight; //to find max height
		StringLine = defaultStringInfo.StringLine;
	}

	vector<ucstring> vstr;
	split(str, '\n', back_inserter(vstr));
	int subStringStart = 0;
	for( vector<ucstring>::size_type i = 0; i < vstr.size(); ++i ) 
	{
		int subStringLen = vstr[i].size();
		CFontManager::instance().textContext(CFontManager::TCGui).printAt (CGuiObject::ToProportionalX(x), 1.0f - CGuiObject::ToProportionalY(y - StringLine + StringHeight), ucstring(ucstring(vstr[i])));

		if(subStringStart <= cursorIndex && cursorIndex <= subStringStart+subStringLen)
		{
			ucstring subSubString = vstr[i].substr(0,cursorIndex-subStringStart);
			float w = CFontManager::instance().getStringWidth(CFontManager::TCGui, ucstring(subSubString));
			cursorPos.y = y - 12 + StringHeight - StringLine + 1;
			cursorPos.x = x + w;
		}

		y += StringHeight + MultilineStringYSpace;
		subStringStart += subStringLen+1;
	}
}
	

CVector CGuiMultilineText::size(bool shaded, int size, const ucstring &str)
{
	CFontManager::instance().textContext(CFontManager::TCGui).setFontSize (size);
	CFontManager::instance().textContext(CFontManager::TCGui).setShaded (shaded);

	static float StringHeight = 0.0f;
	static float StringLine = 0.0f;

	if(StringHeight == 0.0f)
	{
		UTextContext::CStringInfo defaultStringInfo = CFontManager::instance().textContext(CFontManager::TCGui).getStringInfo(ucstring("Yg"));
		StringHeight = defaultStringInfo.StringHeight; //to find max height
		StringLine = defaultStringInfo.StringLine;
	}

	CVector res;
	res.y = StringHeight;
	
	vector<ucstring> vstr;
	split(str, '\n', back_inserter(vstr));
	res.y *= vstr.size();

	if(vstr.size()>1)
		res.y += MultilineStringYSpace * (vstr.size()-1);
	
	res.x = 0;

	for( vector<ucstring>::size_type i = 0; i < vstr.size(); ++i ) 
	{
		float w = CFontManager::instance().getStringWidth(CFontManager::TCGui, ucstring(vstr[i]));
		if(w > res.x)
			res.x = w;
	}
	
	return res;
}
