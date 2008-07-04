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

#ifndef MT_FONT_MANAGER_H
#define MT_FONT_MANAGER_H


//
// Includes
//

#include <nel/3d/u_texture.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_material.h>


//
// Classes
//

class CFontManager : public NLMISC::CSingleton<CFontManager>, public ITask
{
public:

	virtual void init();
	virtual void update() { }
	virtual void render() { }
	virtual void release();

	virtual string name() const { return "CFontManager"; }

	enum TextContext
	{
		TCDebug,
		TCChat,
		TCBig,
		TCPLayerName,
		TCGui,
		NbTC
	};

	// x/y in *character* coord 0/0 is top left and 0/1 is one text line below (not one pixel, one text line)
	void printf(TextContext tc, float x, float y, const char *format, ...);
	void print(TextContext tc, float x, float y, const ucstring &str, bool replaceSmiley = false);
	void print(TextContext tc, const NLMISC::CRGBA &col, float x, float y, const ucstring &str, bool replaceSmiley = false);
	void print3D(TextContext tc, const NLMISC::CRGBA &col, const NLMISC::CVector &pos, const ucstring &str, bool border = true);

	NL3D::UTextContext &textContext(TextContext tc) { return *TC[tc].UTC; }

	uint32 guiFontSize() const { return GUIFontSize; }
	//NL3D::UTextContext	&guiTextContext() const { if(!GUITextContext) throw NLMISC::Exception("NoGUITextContext"); return *GUITextContext; }

	float getStringWidth(TextContext tc, const ucstring &str);

	// return the value that will display the text in the middle of the screen with the TCBig
	uint16 screenCenterX();

private:

	uint32 GUIFontSize;

	struct CTextContext
	{
		NL3D::UTextContext *UTC;
		CHashMap<string, float> Widths;
	};

	CTextContext TC[NbTC];

	struct CReplaceString
	{
		CReplaceString(const ucstring &search)
		{
			Search = search;
		}
		ucstring Search;
		NL3D::UMaterial Material;
		uint32 Width, Height;
	};

	vector<CReplaceString> ReplaceStrings;

	// used to display gfx instead of text (for smiley)
	void drawSpecial(float x, float y, float width, float height, float size, NL3D::UMaterial &material);
	sint32 strfind(const ucstring &str, CReplaceString *&found);
};

#endif
