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

#include <nel/misc/command.h>

#include "3d_task.h"
#include "font_manager.h"
#include "config_file_task.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

void CFontManager::init()
{
	ReplaceStrings.clear();

	vector<string> rep;

	// don't put this in the cfg because we don't want player know them
	rep.push_back(":hud_arrow:");
	rep.push_back("hud_arrow.tga");
	rep.push_back(":hud_gauge:");
	rep.push_back("hud_gauge.tga");

	{
		CConfigFile::CVar &replaceStringVar = CConfigFileTask::instance().configFile().getVar("ReplaceStrings");
		nlassert(replaceStringVar.size() % 2 == 0);
		for(uint i = 0; i < (uint)replaceStringVar.size(); i ++)
			rep.push_back(replaceStringVar.asString(i));
	}

	nlassert(rep.size() % 2 == 0);
	for(uint i = 0; i < (uint)rep.size(); i += 2)
	{
		string search = rep[i + 0];
		string replaceImageFilename = rep[i + 1];

		string fn = CPath::lookup(replaceImageFilename);
		NL3D::UTextureFile *texture = C3DTask::instance().driver().createTextureFile (fn);

		uint32 w, h;
		CBitmap::loadSize(fn, w, h);

		ReplaceStrings.push_back(CReplaceString(ucstring(search)));
		ReplaceStrings.back().Material = C3DTask::instance().createMaterial();
		ReplaceStrings.back().Material.setTexture(texture);
		ReplaceStrings.back().Material.setBlend(true);
		ReplaceStrings.back().Material.setZFunc(UMaterial::always);
		ReplaceStrings.back().Material.setDoubleSided();
		ReplaceStrings.back().Width = w;
		ReplaceStrings.back().Height = h;
//		nlinfo("replace '%s' with '%s'", search.c_str(), replaceImageFilename.c_str());
	}

	// Set the cache size for the font manager (in bytes)
	C3DTask::instance().driver().setFontManagerMaxMemory (2000000);

	// create a Text context for later text rendering
	NL3D::UTextContext *utc = 0;

	GUIFontSize = CConfigFileTask::instance().configFile().getVar("GUIFontSize").asInt();
	utc = C3DTask::instance().driver().createTextContext (CPath::lookup(CConfigFileTask::instance().configFile().getVar("GUIFont").asString()));
	nlassert(utc != 0);
	utc->setKeep800x600Ratio(false);
	utc->setHotSpot (UTextContext::BottomLeft);
	utc->setFontSize (GUIFontSize);
	utc->setColor(CRGBA(0,0,0));
	utc->setShaded(false);
	TC[TCGui].UTC = utc;

	utc = C3DTask::instance().driver().createTextContext (CPath::lookup(CConfigFileTask::instance().configFile().getVar("Font").asString()));
	nlassert(utc != 0);
	utc->setKeep800x600Ratio(false);
	utc->setHotSpot (UTextContext::BottomLeft);
	utc->setFontSize (CConfigFileTask::instance().configFile().getVar("ChatFontSize").asInt());
	utc->setColor(CRGBA(255,255,255,255));
	utc->setShaded(true);
	TC[TCChat].UTC = utc;

	utc = C3DTask::instance().driver().createTextContext (CPath::lookup(CConfigFileTask::instance().configFile().getVar("Font").asString()));
	nlassert(utc != 0);
	utc->setKeep800x600Ratio(false);
	utc->setHotSpot (UTextContext::BottomLeft);
	utc->setFontSize (CConfigFileTask::instance().configFile().getVar("ChatFontSize").asInt());
	utc->setColor(CRGBA(255,255,255,255));
	utc->setShaded(true);
	TC[TCDebug].UTC = utc;

	utc = C3DTask::instance().driver().createTextContext (CPath::lookup(CConfigFileTask::instance().configFile().getVar("Font").asString()));
	nlassert(utc != 0);
	utc->setKeep800x600Ratio(false);
	utc->setHotSpot (UTextContext::BottomLeft);
	utc->setFontSize (CConfigFileTask::instance().configFile().getVar("BigFontSize").asInt());
	utc->setColor(CRGBA(255,255,255,255));
	utc->setShaded(true);
	TC[TCBig].UTC = utc;

	utc = C3DTask::instance().driver().createTextContext (CPath::lookup(CConfigFileTask::instance().configFile().getVar("Font").asString()));
	nlassert(utc != 0);
	utc->setKeep800x600Ratio(false);
	utc->setHotSpot (UTextContext::MiddleBottom);
	utc->setFontSize (CConfigFileTask::instance().configFile().getVar("ChatFontSize").asInt());
	utc->setColor(CRGBA(255,255,255,255));
	utc->setShaded(false);
	TC[TCPLayerName].UTC = utc;
}

void CFontManager::release()
{
}

sint32 CFontManager::strfind(const ucstring &str, CReplaceString *&found)
{
	uint32 minFoundPos = str.size()+1;
	bool isFound = false;
	for(vector<CReplaceString>::iterator it = ReplaceStrings.begin(); it != ReplaceStrings.end(); it++)
	{
		ucstring::size_type sp = 0;

		// we don't want to replace smiley inside the nickname, so we start after it if we found one "<nick> xxx"
		if(str.size() > 0 && (str[0] == '<' || str[0] == '{'))
		{
			ucstring::size_type pos = str.find(ucstring(">"));
			pos = min(pos, str.find(ucstring("}")));
			if (pos != ucstring::npos)
			{
				sp = pos+1;
			}
		}

		ucstring strSearch = (*it).Search;
		while (true)
		{
			ucstring::size_type pos = str.find(strSearch, sp);
			if(pos == ucstring::npos) break;
			if(pos >= minFoundPos) break;

			if((pos == 0 || str[pos-1] == ' ') && (pos+strSearch.size() >= str.size() || str[pos+strSearch.size()] == ' '))
			{
				found = &(*it);
				minFoundPos = pos;
				isFound = true;
				break;
			}
			sp = pos+1;
		}
	}
	if(!isFound)
		return -1;
	return minFoundPos;
}

void CFontManager::drawSpecial(float x, float y, float width, float height, float size, NL3D::UMaterial &material)
{
	float realX = x * size;
	float realY = C3DTask::instance().screenHeight() - ((y+1) * size) - 3;
	float realW = width;
	float realH = height;

	CQuadUV		quad;
	quad.V0.set(realX      , realY      , 0);
	quad.V1.set(realX+realW, realY      , 0);
	quad.V2.set(realX+realW, realY+realH, 0);
	quad.V3.set(realX      , realY+realH, 0);

	quad.Uv3.U = 0;
	quad.Uv3.V = 0;
	quad.Uv2.U = 1;
	quad.Uv2.V = 0;
	quad.Uv1.U = 1;
	quad.Uv1.V = 1;
	quad.Uv0.U = 0;
	quad.Uv0.V = 1;

	C3DTask::instance().driver().setFrustum(CFrustum(0, (float)C3DTask::instance().screenWidth(), 0, (float)C3DTask::instance().screenHeight(), -1, 1, false));
	C3DTask::instance().driver().drawQuad (quad, material);
}

void CFontManager::printf(TextContext tc, float x, float y, const char *format, ...)
{
	H_AUTO2;

	char *str;
	NLMISC_CONVERT_VARGS (str, format, 256);

	uint32 size = textContext(tc).getFontSize();

	float px = (x * size) / (float)C3DTask::instance().screenWidth();
	float py = 1.0f - ((y+1) * size) / (float)C3DTask::instance().screenHeight();

	textContext(tc).printfAt(px, py, str);
}

void CFontManager::print(TextContext tc, const CRGBA &col, float x, float y, const ucstring &str, bool replaceSmiley)
{
	if(str.empty()) return;

	textContext(tc).setColor(col);

	uint32 size = textContext(tc).getFontSize();
	float px = (x * float(size)) / float(C3DTask::instance().screenWidth());
	float py = 1.0f - ((y+1) * size) / float(C3DTask::instance().screenHeight());

	CReplaceString  *replaceString = 0;
	sint32 strFoundOffset = strfind(str, replaceString);

	if(strFoundOffset<0 || !replaceSmiley)
	{
		textContext(tc).printAt(px, py, str);
	}
	else
	{
		uint32 strFoundLength = replaceString->Search.size();
		uint32 strSrcLength = str.size();
		UTextContext::CStringInfo subStringInfo = textContext(tc).getStringInfo(str.substr(0,strFoundOffset));
		
		if(strFoundOffset>0)
			print(tc, col, x, y, str.substr(0, strFoundOffset), replaceSmiley);

		float newStrDx = subStringInfo.StringWidth / size;
		float imageW = (float)replaceString->Width;
		float imageH = (float)replaceString->Height;
		drawSpecial(x+newStrDx, y, imageW, imageH, (float)size, replaceString->Material);
		float specialWidth = imageW / size;

		uint32 dx = strFoundOffset+strFoundLength;
		uint32 remainingCharCount = strSrcLength - dx;
		if(remainingCharCount>0)
			print(tc, col, x+newStrDx+specialWidth, y, str.substr(dx), replaceSmiley);
	}
}

void CFontManager::print(TextContext tc, float x, float y, const ucstring &str, bool replaceSmiley)
{
	print(tc, CRGBA(255,255,255,255), x, y, str, replaceSmiley);
}

void CFontManager::print3D(TextContext tc, const NLMISC::CRGBA &col, const NLMISC::CVector &pos, const ucstring &str, bool border)
{
	H_AUTO2;
	
	C3DTask::instance().driver().setMatrixMode2D11 ();

	CVector p = pos;
	p.z += 1.5f*GScale;

	CMatrix cameraMatrix = C3DTask::instance().scene().getCam().getMatrix();

//	CVector vv = cameraMatrix.getPos() - p;
//	float dt = 1.0f / vv.norm();
//	dt /= 10.0f;

//	if (dt < 0.48f)
//		dt = 0.48f;

	cameraMatrix.invert();

	p = cameraMatrix * p;

	// the text is behind us, don't display it
	if (p.y < 0.0f)
		return;

	p = C3DTask::instance().scene().getCam().getFrustum().project(p);
	
	float fs = float(textContext(tc).getFontSize());
	float x3 = (p.x*(float)C3DTask::instance().screenWidth())/fs;
	float y3 = ((1.0f-p.y) * (float)C3DTask::instance().screenHeight()) / fs - 1;
	if(border)
	{
		CViewport vp = C3DTask::instance().scene().getViewport();
		float onePixelX = 1.0f/vp.getWidth();
		float onePixelY = 1.0f/vp.getHeight();
		float x1 = (p.x*(float)C3DTask::instance().screenWidth()+onePixelX)/fs;
		float y1 = ((1.0f-p.y) * (float)C3DTask::instance().screenHeight()+onePixelY) / fs - 1;
		float x2 = (p.x*(float)C3DTask::instance().screenWidth()-onePixelX)/fs;
		float y2 = ((1.0f-p.y) * (float)C3DTask::instance().screenHeight()-onePixelY) / fs - 1;
		print(tc, CRGBA(0,0,0,col.A), x1, y3, str, false);
		print(tc, CRGBA(0,0,0,col.A), x2, y3, str, false);
		print(tc, CRGBA(0,0,0,col.A), x3, y1, str, false);
		print(tc, CRGBA(0,0,0,col.A), x3, y2, str, false);
	}
	print(tc, col, x3, y3, str, false);
}

float CFontManager::getStringWidth(TextContext tc, const ucstring &str)
{
	CHashMap<string, float>::iterator it = TC[tc].Widths.find(str.toString());
	if(it == TC[tc].Widths.end())
	{
		UTextContext::CStringInfo subStringInfo = CFontManager::instance().textContext(tc).getStringInfo(str);
		TC[tc].Widths[str.toString()] = subStringInfo.StringWidth;
		return subStringInfo.StringWidth;
	}
	else
	{
		return it->second;
	}
}

uint16 CFontManager::screenCenterX() 
{
	return uint16(C3DTask::instance().screenWidth()/2.0f/float(textContext(TCBig).getFontSize()));
}
