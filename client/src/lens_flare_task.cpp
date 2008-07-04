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

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/variable.h>

#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_text_context.h>

#include "3d_task.h"
#include "lens_flare_task.h"
#include "config_file_task.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

/// If axis segment is longer than this value then no lens flare is displayed
static const float MaxLensFlareLength = 0.4f;

class CLensFlare;
static CLensFlare	*LensFlare = 0;

CVector SunDirection(-2.935f, 0.107f, -1.22f);


//
// Classes
//

class CLensFlare
{
	float AlphaCoef;

	/// flare
	struct CFlare
	{
		NL3D::UMaterial Material;

		float Width;
		float Height;

		float Location;

		float Scale;

		CFlare(NL3D::UTexture *texture, float width, float height, float location, float scale)
		{
			Material = C3DTask::instance().createMaterial();
			Material.initUnlit ();
			Material.setTexture (texture);
			Material.setBlendFunc (UMaterial::srcalpha, UMaterial::one);
			Material.setBlend(true);
			Material.setZFunc (UMaterial::always);
			Material.setZWrite (false);

			// quad dimension
			Width = width;
			Height = height;

			// location on the lens-flare ray
			Location = location;

			// texture scale
			Scale = scale;
		}

		~CFlare()
		{
			if(!Material.empty())
			{
				C3DTask::instance().driver().deleteMaterial(Material);
			}
		}
	};

	/// flares due to light
	vector<CFlare *> Flares;

public:

	/// constructor
	CLensFlare()
	{
		AlphaCoef = 1.0f;
	}
	~CLensFlare()
	{
		vector<CFlare *>::iterator it;
		for(it=Flares.begin();it!=Flares.end();it++)
		{
			CFlare *fl = *it;
			delete fl;
		}
		Flares.clear();
	}
		
	/// add a flare to the flare list
	void addFlare(NL3D::UTexture * texture, float width, float height, float location = 1.f, float scale = 1.f)
	{
		Flares.push_back(new CFlare(texture, width, height, location, scale));
	}

	void setAlphaCoef(float coef)
	{
		AlphaCoef = coef;
	}

	/// lens flare display function
	void show();
};

/*********************************************************\
						show()
\*********************************************************/
void CLensFlare::show()
{
	CMatrix mtx;
	mtx.identity();

	C3DTask::instance().driver().setMatrixMode2D11 ();

	// Determining axis "screen center - light" vector
	CMatrix cameraMatrix = C3DTask::instance().scene().getCam().getMatrix();
	cameraMatrix.invert();
	CVector light = (-100000 * SunDirection);
	light = cameraMatrix * light;
	light = C3DTask::instance().scene().getCam().getFrustum().project(light);
	
	CVector screenCenter(0.5f,0.5f,0);
	CVector axis = light - screenCenter;

	if(axis.norm() > MaxLensFlareLength)
	{
		return;
	}

	// rendering flares
	vector<CFlare *>::iterator itflr;
	for(itflr = Flares.begin(); itflr!=Flares.end(); itflr++)
	{
		(*itflr)->Material.setColor(CRGBA(255,255,255,(uint8)(AlphaCoef*255)));
			
		CQuadUV quad;
		
		float xCenterQuad = screenCenter.x + (*itflr)->Location * axis.x;
		float yCenterQuad = screenCenter.y + (*itflr)->Location * axis.y;
		
		float x,y;

		x = xCenterQuad - (*itflr)->Width * (*itflr)->Scale / 2.f;
		y = yCenterQuad - (*itflr)->Height * (*itflr)->Scale / 2.f;
		quad.V0.set (x, y, 0);
		
		x = xCenterQuad + (*itflr)->Width * (*itflr)->Scale / 2.f;
		y = yCenterQuad - (*itflr)->Height * (*itflr)->Scale / 2.f;
		quad.V1.set (x, y, 0);
		
		x = xCenterQuad + (*itflr)->Width * (*itflr)->Scale / 2.f;
		y = yCenterQuad + (*itflr)->Height * (*itflr)->Scale / 2.f;
		quad.V2.set (x, y, 0);
		
		x = xCenterQuad - (*itflr)->Width * (*itflr)->Scale / 2.f;
		y = yCenterQuad + (*itflr)->Height * (*itflr)->Scale / 2.f;
		quad.V3.set (x, y, 0);

		quad.Uv0.U = 0.0f; quad.Uv0.V = 1.0f;
		quad.Uv1.U = 1.0f; quad.Uv1.V = 1.0f;
		quad.Uv2.U = 1.0f; quad.Uv2.V = 0.0f;
		quad.Uv3.U = 0.0f; quad.Uv3.V = 0.0f;

		C3DTask::instance().driver().drawQuad (quad, (*itflr)->Material);
	}
}

CLensFlareTask::CLensFlareTask()
{
	LensFlare = 0;

	flareTexture1 = 0;
	flareTexture3 = 0;
	flareTexture4 = 0;
	flareTexture5 = 0;
	flareTexture6 = 0;
	flareTexture7 = 0;
}

void CLensFlareTask::init()
{
	LensFlare = new CLensFlare ();

	string res;

	res = CPath::lookup("flare01.tga");
	flareTexture1 = C3DTask::instance().driver().createTextureFile (res);
	res = CPath::lookup("flare03.tga");
	flareTexture3 = C3DTask::instance().driver().createTextureFile (res);
	res = CPath::lookup("flare04.tga");
	flareTexture4 = C3DTask::instance().driver().createTextureFile (res);
	res = CPath::lookup("flare05.tga");
	flareTexture5 = C3DTask::instance().driver().createTextureFile (res);
	res = CPath::lookup("flare06.tga");
	flareTexture6 = C3DTask::instance().driver().createTextureFile (res);
	res = CPath::lookup("flare07.tga");
	flareTexture7 = C3DTask::instance().driver().createTextureFile (res);

	float w = 30/800.0f;
	float h = 30/600.0f;

	// shine
	LensFlare->addFlare (flareTexture3, w, h, 1.f, 16.f);
	LensFlare->addFlare (flareTexture1, w, h,  1.0f,  6.0f);
	LensFlare->addFlare (flareTexture6, w, h,  1.3f,  1.2f);
	LensFlare->addFlare (flareTexture7, w, h,  1.0f,  3.0f);
	LensFlare->addFlare (flareTexture6, w, h,  0.5f,  4.0f);
	LensFlare->addFlare (flareTexture5, w, h,  0.2f,  2.0f);
	LensFlare->addFlare (flareTexture7, w, h,  0.0f,  0.8f);
	LensFlare->addFlare (flareTexture7, w, h, -0.25f, 2.0f);
	LensFlare->addFlare (flareTexture1, w, h, -0.4f,  1.0f);
	LensFlare->addFlare (flareTexture4, w, h, -1.0f, 12.0f);
	LensFlare->addFlare (flareTexture5, w, h, -0.6f,  6.0f);

	C3DTask::instance().driver().deleteTextureFile(flareTexture1);
	C3DTask::instance().driver().deleteTextureFile(flareTexture3);
	C3DTask::instance().driver().deleteTextureFile(flareTexture4);
	C3DTask::instance().driver().deleteTextureFile(flareTexture5);
	C3DTask::instance().driver().deleteTextureFile(flareTexture6);
	C3DTask::instance().driver().deleteTextureFile(flareTexture7);

}

void CLensFlareTask::render()
{
	nlassert(LensFlare);
	
//	C3DTask::instance().scene().render();

//	SunDirection.x = CConfigFileTask::instance().configFile().getVar("sx").asFloat();
//	SunDirection.y = CConfigFileTask::instance().configFile().getVar("sy").asFloat();
//	SunDirection.z = CConfigFileTask::instance().configFile().getVar("sz").asFloat();

	// vector to sun
	//==============
	CVector userLook = C3DTask::instance().scene().getCam().getMatrix().getJ();

	CVector sunDirection = (-100000 * SunDirection);

	// cosinus between the two previous vectors
	//=========================================
	float cosAngle = sunDirection*userLook/sunDirection.norm();	

	// alpha
	//======
	float alphaf;
	if(cosAngle<0) 
	{
		alphaf = 0;
	}
	else
	{
		alphaf = 255*(float)(pow(cosAngle,20));
	}

	// landscape's masking sun ?
	//==========================
	CMatrix camMatrix;
	camMatrix = C3DTask::instance().scene().getCam().getMatrix();
	camMatrix.setPos(CVector::Null);
	camMatrix.invert();
	CVector tmp = camMatrix * sunDirection;
	tmp = C3DTask::instance().scene().getCam().getFrustum().project(tmp);
	uint32	w,h;
	C3DTask::instance().driver().getWindowSize(w,h);
	float sunRadius = 24;
	CRect rect((uint32)(tmp.x*w)-(uint32)sunRadius,(uint32)(tmp.y*h)-(uint32)sunRadius,2*(uint32)sunRadius,2*(uint32)sunRadius);
	vector<float> zbuff;
	C3DTask::instance().driver().getZBufferPart(zbuff, rect);
	float view = 0.f;
	float sum = 0;
	sint i;
	for(i=0; i<(sint)zbuff.size(); i++)
	{
		if(zbuff[i]>=0.99999f) sum ++;
	}
	view = sum/(sunRadius*2*sunRadius*2);

	C3DTask::instance().driver().setMatrixMode2D11 ();

	// quad for dazzle 
	//================
	uint8 alpha = (uint8)(alphaf*view/2.0f);
	if(alpha!=0)
	{
		C3DTask::instance().driver().drawQuad(0,0,1,1,CRGBA(255,255,255,alpha));
	}

	// Display lens-flare
	LensFlare->setAlphaCoef( 1.f - (float)cos(alphaf*view*Pi/(2.f*255.f)) );
	LensFlare->show();
}

void CLensFlareTask::release()
{
	if(LensFlare)
	{
		delete LensFlare;
		LensFlare = 0;
	}
}
