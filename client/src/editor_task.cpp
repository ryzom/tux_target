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

#include <nel/misc/variable.h>
#include <nel/3d/u_material.h>

#include "level.h"
#include "3d_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "editor_task.h"
#include "network_task.h"
#include "level_manager.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

static CVector rayTestStart,rayTestEnd;
static UMaterial testMat;

CVariable<bool> EnableEditor("variables","EnableEditor", "1 if you want a key to enable the editor", false, 0, true);


//
// Functions
//


void CEditorTask::init()
{
	_enable = false;
	_testFrame = 0;
	_selectedElement = 0;
	_mouseX = 0;
	_mouseY = -0.5f;
	_lastUpdateTime = 0;

	rayTestStart = CVector(0,0,0);
	rayTestEnd= CVector(0,0,0);
	testMat = C3DTask::instance().createMaterial();
	testMat.setColor(CRGBA(255,255,255,255));
	testMat.setZWrite(true);
	testMat.setZFunc(UMaterial::always);

}


void CEditorTask::_mouseSelectModule() 
{
	bool mousePressed = CGuiObjectManager::instance().mouseListener().Pressed;
	CVector mousePressedPos = CGuiObjectManager::instance().mouseListener().pressedPosition();
	uint32 i;
	if(mousePressed && CLevelManager::instance().levelPresent())
	{
		_selectedElement = 0;
		//nlinfo("editor ray test");
		CViewport vp = C3DTask::instance().scene().getViewport();
		CMatrix camMat = C3DTask::instance().scene().getCam().getMatrix();
		CFrustum frustum = C3DTask::instance().scene().getCam().getFrustum();
		C3DTask::instance().driver().setFrustum(frustum);
		CVector rayStart,rayDir,rayEnd;
		float x = mousePressedPos.x / C3DTask::instance().screenWidth();
		float y = 1.0f - mousePressedPos.y / C3DTask::instance().screenHeight();
		vp.getRayWithPoint(x,y,rayStart,rayDir,camMat,frustum);
		rayDir.normalize();
		rayEnd = rayStart + rayDir * 10000;

		rayTestEnd = rayEnd;
		rayTestStart = rayStart;

		list<CEditableElementCommon *> bboxModules;
		for(i=0;i<CLevelManager::instance().currentLevel().moduleCount();i++)
		{
			CModule *module = CLevelManager::instance().currentLevel().module(i);
			UInstance mesh = module->mesh();
			CAABBox bbox;
			mesh.getShapeAABBox(bbox);
			CAABBox tbbox = CAABBox::transformAABBox(mesh.getMatrix(),bbox);
			bool intersect = tbbox.intersect(rayStart,rayEnd);
			if(intersect)
			{
				bboxModules.push_back(module);
				nlinfo("bbox intersect %s 0x%p ",module->name().c_str(),module);
			}
		}

		for(i=0;i<CLevelManager::instance().currentLevel().getStartPointCount();i++)
		{
			CStartPoint *startpoint = CLevelManager::instance().currentLevel().getStartPoint(i);
			UInstance mesh = startpoint->mesh();
			CAABBox bbox;
			mesh.getShapeAABBox(bbox);
			CAABBox tbbox = CAABBox::transformAABBox(mesh.getMatrix(),bbox);
			bool intersect = tbbox.intersect(rayStart,rayEnd);
			if(intersect)
			{
				bboxModules.push_back(startpoint);
				nlinfo("bbox intersect %s 0x%p ",startpoint->name().c_str(),startpoint);
			}
		}

		//test plus precis :

		CEditableElementCommon *nearestElement = 0;
		float nearestElmentDist = 10000.0f;
		list<CEditableElementCommon *>::iterator it;
		for(it=bboxModules.begin();it!=bboxModules.end();it++)
		{
			CEditableElementCommon *element = *it;
			CVector rayHit;
			NL3D :: UInstance mesh = element->mesh();
			CMatrix mat = mesh.getMatrix();
			bool intersect = element->intersect(rayStart,rayEnd,rayHit,mat);
			if(intersect)
			{
				CVector rayDirHit = rayHit - rayStart;
				float dist = rayDirHit * rayDir;
				if(dist>0.0f && dist<nearestElmentDist)
				{
					nearestElmentDist = dist;
					nearestElement = element;
				}
				nlinfo("tri intersect %s 0x%p dist = %f (%f %f %f)",nearestElement->name().c_str(),nearestElement,nearestElmentDist,rayHit.x,rayHit.y,rayHit.z);
			}
		}

		if(nearestElement && _selectedElement != nearestElement)
		{
			nlinfo("tri intersect %s 0x%p dist = %f",nearestElement->name().c_str(),nearestElement,nearestElmentDist);
			_selectedElement = nearestElement;		
		}
	}
}

void CEditorTask::update() 
{

/*	if(EnableEditor && C3DTask::instance().kbPressed(KeyFXX))
	{
		enable(!enable());
	}*/

	if(_enable && CLevelManager::instance().levelPresent())
	{
		_mouseSelectModule();


		if (C3DTask::instance().kbPressed(KeySPACE) && C3DTask::instance().kbDown(KeyCONTROL))
		{
			CMatrix camMat = C3DTask::instance().scene().getCam().getMatrix();
			CVector camDir = camMat.getJ();
			CAABBox bbox;
			_selectedElement->mesh().getShapeAABBox(bbox);
			ControlerFreeLookPos = _selectedElement->position()  - camDir * (bbox.getRadius() + 0.1f);
		}


		if (C3DTask::instance().kbPressed(KeyNUMPAD4))
		{
			_mouseX += (float)Pi / 8.0f;
		}
		if (C3DTask::instance().kbPressed(KeyNUMPAD6))
		{
			_mouseX -= (float)Pi / 8.0f;
		}
		if (C3DTask::instance().kbPressed(KeyNUMPAD8))
		{
			_mouseY += 0.1f;
		}
		if (C3DTask::instance().kbPressed(KeyNUMPAD5))
		{
			_mouseY -= 0.1f;
		}

		CVector dv(0,0,0);
		if (C3DTask::instance().kbDown(KeyUP))
		{
			dv.y += 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyDOWN))
		{
			dv.y -= 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyLEFT))
		{
			dv.x -= 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyRIGHT))
		{
			dv.x += 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyPRIOR))
		{
			dv.z += 1.0f;
		}
		if (C3DTask::instance().kbDown(KeyNEXT))
		{
			dv.z -= 1.0f;
		}

		dv /= 4.0f;

		if (C3DTask::instance().kbDown(KeySHIFT))
			dv *= 0.1f;

		if (C3DTask::instance().kbDown(KeyCONTROL))
			dv *= 20.0f;

		dv *= (float)CTimeTask::instance().deltaTime();

		//float	mouseX = 0;//CGuiObjectManager::instance().mouseListener().MouseX;
		//float	mouseY = -0.5f;//-CGuiObjectManager::instance().mouseListener().MouseY;

		CMatrix m2;
		m2.identity();
		m2.rotateZ(_mouseX);
		//m2.rotateX(mouseY);
		dv = m2 * dv;
		ControlerFreeLookPos += dv;

		ControlerCamMatrix.identity();
		ControlerCamMatrix.translate(ControlerFreeLookPos);
		ControlerCamMatrix.rotateZ(_mouseX);
		ControlerCamMatrix.rotateX(_mouseY);
		//			nlinfo("set camera matrix");
		C3DTask::instance().scene().getCam().setMatrix(ControlerCamMatrix);			

		if(selectedElement() && dv.norm()!=0.0f)
		{
			CVector oldPos = selectedElement()->position();
			selectedElement()->setPosition(oldPos + dv);
			if(selectedElement()->type()==CEditableElementCommon::Module)
			{
				for(uint i=0;i<CLevelManager::instance().currentLevel().moduleCount();i++)
				{
					CModule *module = CLevelManager::instance().currentLevel().module(i);
					if(module && module!=selectedElement())
					{
						/*
						CVector newPos;
						CAngleAxis newRot;
						bool res = CAutoEdge::compute(module,selectedElement(),newPos,newRot);
						if(res)
						selectedElement()->position(newPos);
						*/
						CVector translation;
						CVector rotation;
						bool res = CAutoEdge::compute(module,selectedElement(),translation,rotation);
						if(res)
							selectedElement()->setPosition(oldPos + dv + translation);
					}
				}
			}

		}

		double time = CTimeTask::instance().time();
		if((time-_lastUpdateTime)>0.1f)
		{
			for(uint i=0;i<CLevelManager::instance().currentLevel().moduleCount();i++)
			{
				CModule *m = CLevelManager::instance().currentLevel().module(i);
// 				if(m->changed())
// 				{
// 					CNetworkTask::instance().updateEditableElement(m);
// 					m->changed(false);
// 				}
			}
			for(uint i=0;i<CLevelManager::instance().currentLevel().getStartPointCount();i++)
			{
				CStartPoint *m = CLevelManager::instance().currentLevel().getStartPoint(i);
// 				if(m->changed())
// 				{
// 					CNetworkTask::instance().updateEditableElement(m);
// 					m->changed(false);
// 				}
			}
			_lastUpdateTime = time;
		}
	}
}

void CEditorTask::render()
{
	// TODO pourquoi cette task fait deconner les rendu drawQuad???
/*	C3DTask::instance().driver().setFrustum(C3DTask::instance().scene().getCam().getFrustum());
	UCamera cam = C3DTask::instance().scene().getCam();
	C3DTask::instance().driver().setMatrixMode3D(cam);
	if(_selectedElement)
	{
		_selectedElement->renderSelection();
	}

	CLine l;

	l.V0 = rayTestStart;
	l.V1 = rayTestEnd;
	//C3DTask::instance().driver().drawLine(l,*testMat);
*/}

void CEditorTask::release()
{
}

void CEditorTask::enable(bool e)
{
	if(e==_enable) return;
	if(e)
	{
//		nlassert(!_testFrame);
//		guiSPG<CGuiXml> xml = 0;
//		xml = CGuiXmlManager::instance().load("editor.xml");
//		_testFrame = (CGuiFrame *)xml->get("editorFrame");
//		CGuiObjectManager::instance().addFrame(_testFrame);
	}
	else
	{
		_testFrame = 0;
	}
	_enable = e;
	FollowEntity = !e;
	CNetworkTask::instance().setEditMode(_enable?1:0);
	//	CMtpTarget::instance().controler().Camera.getMatrix()->getPos(ControlerFreeLookPos);
}

bool CEditorTask::enable()
{
	return _enable;
}

void CEditorTask::reset()
{
	_selectedElement = 0;
}
