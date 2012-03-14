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
// Includes
//

#include "stdpch.h"

#include "3d_task.h"
#include "font_manager.h"
#include "config_file_task.h"
#include "resource_manager2.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//

string CharOrder = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:-$@[]";
uint32 CharX[] =
{
	35,  62,  88, 111, 137, 163, 187, 212,
	36,  49,  69,  96, 119, 150, 177, 204,
	35,  62,  87, 113, 143, 168, 195, 229,
	35,  61,  85, 109, 124, 150, 174, 199, 224,
	35,  59,  84, 110, 122, 134, 155, 176, 195, 204
};
uint32 CharY[] =
{
	123, 123, 123, 123, 123, 123, 123, 123,
	145, 145, 145, 145, 145, 145, 145, 145,
	166, 166, 166, 166, 166, 166, 166, 166,
	188, 188, 188, 188, 188, 188, 188, 188, 188,
	210, 210, 210, 210, 210, 210, 210, 210, 210, 210
};
uint32 CharW[] =
{
	22,  22,  22,  22,  22,  22,  22,  22,
	12,  19,  25,  22,  29,  25,  25,  22,
	22,  25,  22,  25,  22,  22,  30,  23,
	22,  22,  22,  14,  22,  22,  22,  22,  22,
	22,  22,  22,  10,  10,  17,  21,  17, 9, 9
};
uint32 CharH[] =
{
	22,  22,  22,  22,  22,  22,  22,  22,
	21,  21,  21,  21,  21,  21,  21,  21,
	22,  22,  22,  22,  22,  22,  22,  22,
	22,  22,  22,  22,  22,  22,  22,  22,  22,
	22,  22,  22,  22,  22,  22,  22,  22,  22, 22
};

//
// Functions
//

void CFontManager::init()
{
	replaceStrings.clear();
	CConfigFile::CVar &replaceStringVar = CConfigFileTask::getInstance().configFile().getVar("ReplaceStringByImage");
	nlassert(replaceStringVar.size() % 2 == 0);
	for(uint i = 0; i < (uint)replaceStringVar.size(); i += 2)
	{
		string search = replaceStringVar.asString(i+0);
		string replaceImageFilename = replaceStringVar.asString(i+1);
		
		string res = CResourceManager::getInstance().get(replaceImageFilename);
		NL3D::UTextureFile *texture = C3DTask::getInstance().driver().createTextureFile (res);
		CReplaceString *replaceString = new CReplaceString(search);
		replaceString->material = C3DTask::getInstance().createMaterial();
		replaceString->material.setTexture(texture);
		replaceString->material.setBlend(true);
		replaceString->material.setZFunc(UMaterial::always);
		replaceString->material.setDoubleSided();
		replaceStrings.push_back(replaceString);
		nlinfo("%s->%s(0x%p)",search.c_str(),replaceImageFilename.c_str(),&(replaceString->material));
	}

	string res = CResourceManager::getInstance().get("font.tga");
	Texture = C3DTask::getInstance().driver().createTextureFile (res);
	Material = C3DTask::getInstance().createMaterial();
	Material.setTexture(Texture);
	Material.setBlend(true);
	Material.setZFunc(UMaterial::always);
	Material.setDoubleSided();

	FontWidth = 22;
	FontHeight = 20;

	// Set the cache size for the font manager (in bytes)
	C3DTask::getInstance().driver().setFontManagerMaxMemory (2000000);
	
	// Create a Text context for later text rendering
	//	TextContext = Driver->createTextContext (CPath::lookup("n019003l.pfb"));
	LittleTextContext = C3DTask::getInstance().driver().createTextContext (CPath::lookup(CConfigFileTask::getInstance().configFile().getVar("LittleFont").asString()));
	nlassert(LittleTextContext != 0);
	LittleTextContext->setKeep800x600Ratio(false);
	LittleTextContext->setHotSpot (UTextContext::BottomLeft);
	LittleTextContext->setFontSize (12);
	LittleTextContext->setShaded(true);

	BigFontSize = CConfigFileTask::getInstance().configFile().getVar("BigFontSize").asInt();
	BigTextContext = C3DTask::getInstance().driver().createTextContext (CPath::lookup(CConfigFileTask::getInstance().configFile().getVar("BigFont").asString()));
	nlassert(BigTextContext != 0);
	BigTextContext->setKeep800x600Ratio(false);
	BigTextContext->setHotSpot (UTextContext::BottomLeft);
	BigTextContext->setFontSize (BigFontSize);
	BigTextContext->setShaded(true);
	
	GUIFontSize = CConfigFileTask::getInstance().configFile().getVar("GUIFontSize").asInt();
	GUITextContext = C3DTask::getInstance().driver().createTextContext (CPath::lookup(CConfigFileTask::getInstance().configFile().getVar("GUIFont").asString()));
	nlassert(GUITextContext != 0);
	GUITextContext->setKeep800x600Ratio(false);
	GUITextContext->setHotSpot (UTextContext::BottomLeft);
	GUITextContext->setFontSize (GUIFontSize);
	GUITextContext->setColor(CRGBA(0,0,0));
	GUITextContext->setShaded(false);
}

void CFontManager::release()
{
	if(!Material.empty())
	{
		C3DTask::getInstance().driver().deleteMaterial(Material);
	}
	if(Texture)
	{
		C3DTask::getInstance().driver().deleteTextureFile(Texture);
		Texture = 0;
	}
}

sint32 CFontManager::strfind(string &str,CReplaceString **found)
{
	vector<CReplaceString *>::iterator it;
	uint32 minFoundPos = str.size()+1;
	bool isFound = false;
	for(it=replaceStrings.begin();it!=replaceStrings.end();it++)
	{
		string strSearch = (*it)->search;
		string::size_type pos = str.find(strSearch);
		if(pos!=string::npos && minFoundPos>pos)
		{
			*found = *it;
			minFoundPos = pos;
			isFound = true;
		}
	}
	if(!isFound)
		return -1;
	return minFoundPos;
}

void CFontManager::drawSpecial(float x, float y,float width,float height, NL3D::UMaterial &material)
{
	float realX = 8*x;
	float realY = C3DTask::getInstance().screenHeight()-(16*(y+1)+4);
	float realW = width;
	float realH = height;

	CQuadUV		quad;
	quad.V0.set(realX      , realY      , 0);
	quad.V1.set(realX+realW, realY      , 0);
	quad.V2.set(realX+realW, realY+realH, 0);
	quad.V3.set(realX      , realY+realH, 0);

	quad.Uv3.U= 0;
	quad.Uv3.V= 0;
	quad.Uv2.U= 1;
	quad.Uv2.V= 0;
	quad.Uv1.U= 1;
	quad.Uv1.V= 1;
	quad.Uv0.U= 0;
	quad.Uv0.V= 1;

	
	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	C3DTask::getInstance().driver().drawQuad (quad, material);

}

void CFontManager::littlePrintf(const CRGBA &col, float x, float y, const char *format ...)
{
	H_AUTO(littlePrintf2);

	char *str;
	NLMISC_CONVERT_VARGS (str, format, 256);

	LittleTextContext->setColor (col);
	LittleTextContext->printfAt (8*x/C3DTask::getInstance().screenWidth(), (C3DTask::getInstance().screenHeight()-16*y-16)/C3DTask::getInstance().screenHeight(), str);
}

void CFontManager::littlePrintf(float x, float y, const char *format ...)
{
	H_AUTO(littlePrintf);

	char *str;
	NLMISC_CONVERT_VARGS (str, format, 256);

	CReplaceString  *replaceString;
	string strtmp = str;
	sint32 strFoundOffset = strfind(strtmp,&replaceString);

	if(strFoundOffset<0)
	{
		LittleTextContext->setColor (CRGBA (255,255,255,255));
		LittleTextContext->printfAt (8*x/C3DTask::getInstance().screenWidth(), (C3DTask::getInstance().screenHeight()-16*y-16)/C3DTask::getInstance().screenHeight(), str);
	}
	else
	{
		uint32 strFoundLength = replaceString->search.size();
		uint32 strSrcLength = strlen(str);
		UTextContext::CStringInfo subStringInfo = CFontManager::getInstance().guiTextContext().getStringInfo(ucstring(string(str).substr(0,strFoundOffset)));
		
		if(strFoundOffset>0)
			littlePrint(x,y,strFoundOffset,str);

		float newStrDx = subStringInfo.StringWidth/8;
		float imageW = 16;
		float imageH = 16;
		drawSpecial(x+newStrDx,y,imageW,imageH,replaceString->material);
		float specialWidth = imageW / 8;

		uint32 dx = strFoundOffset+strFoundLength;
		uint32 remainingCharCount = strSrcLength - dx;
		if(remainingCharCount>0)
			littlePrintf(x+newStrDx+specialWidth,y,str+dx);
	}
}

void CFontManager::littlePrint(float x, float y, uint32 count, const char *ch)
{
	string str = ch;

	LittleTextContext->setColor (CRGBA (255,255,255,255));
	LittleTextContext->printfAt (8*x/C3DTask::getInstance().screenWidth(), (C3DTask::getInstance().screenHeight()-16*y-16)/C3DTask::getInstance().screenHeight(), str.substr(0,count).c_str());
}

void CFontManager::printf(const NLMISC::CRGBA &col, float x, float y, float scale, const char *format, ...)
{
	H_AUTO(printf);

	char *str;
	NLMISC_CONVERT_VARGS (str, format, 256);
	
	float px;
	
	float py;
	px = x;
	py = y+FontHeight;
	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	Material.setColor(col);

	for (char *ptr = str; *ptr; ptr++)
	{
		*ptr = toupper(*ptr);

		if (*ptr == ' ')
		{
			px += 22*scale;
		}
		else if(*ptr == '\n')
		{
			px += x;
			py += 22*scale;
		}
		else
		{
			for(uint j = 0; j < CharOrder.size(); j++)
			{
				if (CharOrder[j] == *ptr)
				{
					CQuadUV		quad;
					quad.V0.set(px,C3DTask::getInstance().screenHeight()-py,0);
					quad.V1.set(px+CharW[j]*scale,C3DTask::getInstance().screenHeight()-py,0);
					quad.V2.set(px+CharW[j]*scale,C3DTask::getInstance().screenHeight()-py+CharH[j]*scale,0);
					quad.V3.set(px,C3DTask::getInstance().screenHeight()-py+CharH[j]*scale,0);

					int rx1 = CharX[j]+3;
					int ry1 = CharY[j]+23;
					int rx2 = CharX[j]+CharW[j]+3;
					int ry2 = CharY[j]+CharH[j]+23;
					
					quad.Uv0.U= rx1/256.0f;
					quad.Uv0.V= ry2/256.0f;
					quad.Uv1.U= rx2/256.0f;
					quad.Uv1.V= ry2/256.0f;
					quad.Uv2.U= rx2/256.0f;
					quad.Uv2.V= ry1/256.0f;
					quad.Uv3.U= rx1/256.0f;
					quad.Uv3.V= ry1/256.0f;

//					C3DTask::getInstance().driver().drawQuad (quad, Material);
					px += CharW[j]*scale;
					break;
				}
			}
		}
	}

	BigTextContext->setColor (col);
	BigTextContext->printfAt (x/(float)C3DTask::getInstance().screenWidth(), 1.0f-y/(float)C3DTask::getInstance().screenHeight(), str);
}


void CFontManager::printf3D(const NLMISC::CRGBA &col, const NLMISC::CVector &pos, float scale, const char *format, ...)
{
	H_AUTO(printf3D);
	
	char *str;
	NLMISC_CONVERT_VARGS (str, format, 256);

	C3DTask::getInstance().driver().setMatrixMode2D11 ();

	CVector p = pos;
	p.z += 0.02f;
	
	CMatrix cameraMatrix = C3DTask::getInstance().scene().getCam().getMatrix();

	CVector vv = cameraMatrix.getPos() - p;
	float dt = 1.0f / vv.norm();
	dt /= 10.0f;

	if (dt < 0.48f)
		dt = 0.48f;
	
	cameraMatrix.invert();

	p = cameraMatrix * p;

	// the text is behind us, don't display it
	if (p.y < 0.0f)
		return;

	p = C3DTask::getInstance().scene().getCam().getFrustum().project(p);

	float avx = -(float(strlen(str))/2.0f*22.0f*scale/10.0f);

	Material.setZFunc(UMaterial::lessequal);
	Material.setColor(col);
	
	for (char *ptr = str; *ptr; ptr++)
	{
		*ptr = toupper(*ptr);
		
		if (*ptr == ' ')
		{
			avx += 22.0f*scale/10.0f;
		}
		else
		{
			for(uint j = 0; j < CharOrder.size(); j++)
			{
				if (CharOrder[j] == *ptr)
				{
					CQuadUV q;
					q.V0.set(p.x+(avx-0.01f)*dt, p.y-0.01f*dt*(4.0f/3.0f), p.z);
					q.V1.set(p.x+(avx-0.01f)*dt, p.y+0.01f*dt*(4.0f/3.0f), p.z);
					q.V2.set(p.x+(avx+0.01f)*dt, p.y+0.01f*dt*(4.0f/3.0f), p.z);
					q.V3.set(p.x+(avx+0.01f)*dt, p.y-0.01f*dt*(4.0f/3.0f), p.z);

					float rx1 = CharX[j]+3+0.5f;
					float ry1 = CharY[j]+23+0.5f;
					float rx2 = CharX[j]+CharW[j]+3-0.5f;
					float ry2 = CharY[j]+CharH[j]+23-0.5f;
					
					q.Uv2.U= rx2/256.0f;
					q.Uv2.V= ry1/256.0f;
					q.Uv3.U= rx2/256.0f;
					q.Uv3.V= ry2/256.0f;
					q.Uv0.U= rx1/256.0f;
					q.Uv0.V= ry2/256.0f;
					q.Uv1.U= rx1/256.0f;
					q.Uv1.V= ry1/256.0f;	

					C3DTask::getInstance().driver().drawQuad (q, Material);

					avx += CharW[j]*scale/10.0f;
					break;
				}
			}
		}
	}

	Material.setZFunc(UMaterial::always);
}
