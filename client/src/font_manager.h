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

#ifndef MTPT_FONT_MANAGER_H
#define MTPT_FONT_MANAGER_H


//
// Includes
//

#include <nel/3d/u_texture.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_material.h>


//
// Classes
//

class CReplaceString
{
public:
	CReplaceString(std::string &search)
	{
		this->search = search;
	}
	std::string search;
	NL3D::UMaterial material;
protected:
private:
};

class CFontManager : public NLMISC::CSingleton<CFontManager>, public ITask
{
public:

	virtual void init();
	virtual void update() { }
	virtual void render() { }
	virtual void release();

	virtual std::string name() const { return "CFontManager"; }

	NL3D::UMaterial &material() { return Material; }

	void littlePrintf(float x, float y, const char *format ...);
	void littlePrintf(const NLMISC::CRGBA &col, float x, float y, const char *format ...);
	void littlePrint(float x, float y, uint32 count, const char *ch);
	void printf(const NLMISC::CRGBA &col, float x, float y, float scale, const char *format, ...);
	void printf3D(const NLMISC::CRGBA &col, const NLMISC::CVector &pos, float scale, const char *format, ...);
	
	uint32 fontWidth() const { return FontWidth; }
	uint32 fontHeight() const { return FontHeight; }

	uint32 guiFontSize() const { return GUIFontSize; }

	NL3D::UTextContext	&guiTextContext() const { nlassert(GUITextContext); return *GUITextContext; }

private:

	void drawSpecial(float x, float y,float width,float height, NL3D::UMaterial &material);
	sint32 strfind(std::string &str,CReplaceString **found);
	uint32 FontHeight;
	uint32 FontWidth;

	uint32 GUIFontSize, BigFontSize;

	NL3D::UTextContext	*LittleTextContext, *BigTextContext, *GUITextContext;
	
	NL3D::UTextureFile	*Texture;
	NL3D::UMaterial		 Material;

	std::vector<CReplaceString *> replaceStrings;

};

#endif
