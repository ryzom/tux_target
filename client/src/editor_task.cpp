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

#include <string>

#include <nel/misc/variable.h>
#include <nel/3d/u_material.h>

#include "editor_task.h"
#include "time_task.h"
#include "3d_task.h"
#include "network_task.h"
#include "level_manager.h"
#include "mtp_target.h"


using namespace std;
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
	testMat = C3DTask::getInstance().createMaterial();
	testMat.setColor(CRGBA(255,255,255,255));
	testMat.setZWrite(true);
	testMat.setZFunc(UMaterial::always);
	
}


void CEditorTask::_mouseSelectModule() 
{
	bool mousePressed = CGuiObjectManager::getInstance().mouseListener().Pressed;
	CVector mousePressedPos = CGuiObjectManager::getInstance().mouseListener().pressedPosition();
	uint32 i;
	if(mousePressed && CLevelManager::getInstance().levelPresent())
	{
		_selectedElement = 0;
		//nlinfo("editor ray test");
		CViewport vp = C3DTask::getInstance().scene().getViewport();
		CMatrix camMat = C3DTask::getInstance().scene().getCam().getMatrix();
		CFrustum frustum = C3DTask::getInstance().scene().getCam().getFrustum();
		C3DTask::getInstance().driver().setFrustum(frustum);
		CVector rayStart,rayDir,rayEnd;
		float x = mousePressedPos.x / C3DTask::getInstance().screenWidth();
		float y = 1.0f - mousePressedPos.y / C3DTask::getInstance().screenHeight();
		vp.getRayWithPoint(x,y,rayStart,rayDir,camMat,frustum);
		rayDir.normalize();
		rayEnd = rayStart + rayDir * 10000;

		rayTestEnd = rayEnd;
		rayTestStart = rayStart;
		
		list<CEditableElementCommon *> bboxModules;
		for(i=0;i<CLevelManager::getInstance().currentLevel().getModuleCount();i++)
		{
			CModule *module = CLevelManager::getInstance().currentLevel().getModule(i);
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
		
		for(i=0;i<CLevelManager::getInstance().currentLevel().getStartPointCount();i++)
		{
			CStartPoint *startpoint = CLevelManager::getInstance().currentLevel().getStartPoint(i);
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

	if(EnableEditor && C3DTask::getInstance().kbPressed(KeyF4))
	{
		enable(!enable());
	}

	if(_enable && CLevelManager::getInstance().levelPresent())
	{
		_mouseSelectModule();


		if (C3DTask::getInstance().kbPressed(KeySPACE) && C3DTask::getInstance().kbDown(KeyCONTROL))
		{
			CMatrix camMat = C3DTask::getInstance().scene().getCam().getMatrix();
			CVector camDir = camMat.getJ();
			CAABBox bbox;
			_selectedElement->mesh().getShapeAABBox(bbox);
			ControlerFreeLookPos = _selectedElement->position()  - camDir * (bbox.getRadius() + 0.1f);
		}
		

		if (C3DTask::getInstance().kbPressed(KeyNUMPAD4))
		{
			_mouseX += (float)Pi / 8.0f;
		}
		if (C3DTask::getInstance().kbPressed(KeyNUMPAD6))
		{
			_mouseX -= (float)Pi / 8.0f;
		}
		if (C3DTask::getInstance().kbPressed(KeyNUMPAD8))
		{
			_mouseY += 0.1f;
		}
		if (C3DTask::getInstance().kbPressed(KeyNUMPAD5))
		{
			_mouseY -= 0.1f;
		}
		
		CVector dv(0,0,0);
		if (C3DTask::getInstance().kbDown(KeyUP))
		{
			dv.y += 1.0f;
		}
		if (C3DTask::getInstance().kbDown(KeyDOWN))
		{
			dv.y -= 1.0f;
		}
		if (C3DTask::getInstance().kbDown(KeyLEFT))
		{
			dv.x -= 1.0f;
		}
		if (C3DTask::getInstance().kbDown(KeyRIGHT))
		{
			dv.x += 1.0f;
		}
		if (C3DTask::getInstance().kbDown(KeyPRIOR))
		{
			dv.z += 1.0f;
		}
		if (C3DTask::getInstance().kbDown(KeyNEXT))
		{
			dv.z -= 1.0f;
		}
		
		dv /= 4.0f;
		
		if (C3DTask::getInstance().kbDown(KeySHIFT))
			dv *= 0.1f;
		
		if (C3DTask::getInstance().kbDown(KeyCONTROL))
			dv *= 20.0f;
		
		dv *= (float)CTimeTask::getInstance().deltaTime();

		//float	mouseX = 0;//CGuiObjectManager::getInstance().mouseListener().MouseX;
		//float	mouseY = -0.5f;//-CGuiObjectManager::getInstance().mouseListener().MouseY;

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
		C3DTask::getInstance().scene().getCam().setMatrix(ControlerCamMatrix);			

		if(selectedElement() && dv.norm()!=0.0f)
		{
			CVector oldPos = selectedElement()->position();
			selectedElement()->position(oldPos + dv);
			if(selectedElement()->type()==CEditableElementCommon::Module)
			{
				for(uint i=0;i<CLevelManager::getInstance().currentLevel().getModuleCount();i++)
				{
					CModule *module = CLevelManager::getInstance().currentLevel().getModule(i);
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
							selectedElement()->position(oldPos + dv + translation);
						
					}
				}
			}
			
		}

		double time = CTimeTask::getInstance().time();
		if((time-_lastUpdateTime)>0.1f)
		{
			for(uint i=0;i<CLevelManager::getInstance().currentLevel().getModuleCount();i++)
			{
				CModule *m = CLevelManager::getInstance().currentLevel().getModule(i);
				if(m->changed())
				{
					CNetworkTask::getInstance().updateEditableElement(m);
					m->changed(false);
				}
			}
			for(uint i=0;i<CLevelManager::getInstance().currentLevel().getStartPointCount();i++)
			{
				CStartPoint *m = CLevelManager::getInstance().currentLevel().getStartPoint(i);
				if(m->changed())
				{
					CNetworkTask::getInstance().updateEditableElement(m);
					m->changed(false);
				}
			}
			_lastUpdateTime = time;
		}
	}
}

void CEditorTask::render()
{
	C3DTask::getInstance().driver().setFrustum(C3DTask::getInstance().scene().getCam().getFrustum());
	UCamera cam = C3DTask::getInstance().scene().getCam();
	C3DTask::getInstance().driver().setMatrixMode3D(cam);
	if(_selectedElement)
	{
		_selectedElement->renderSelection();
	}

	CLine l;
	
	l.V0 = rayTestStart;
	l.V1 = rayTestEnd;
	//C3DTask::getInstance().driver().drawLine(l,*testMat);
	
}

void CEditorTask::release()
{
}

void CEditorTask::enable(bool e)
{
	if(e==_enable) return;
	if(e)
	{
		nlassert(!_testFrame);
		guiSPG<CGuiXml> xml = 0;
		xml = CGuiXmlManager::getInstance().Load("editor.xml");
		_testFrame = (CGuiFrame *)xml->get("editorFrame");
		CGuiObjectManager::getInstance().objects.push_back(_testFrame);	
	}
	else
	{
		_testFrame = 0;
	}
	_enable = e;
	FollowEntity = !e;
	CNetworkTask::getInstance().setEditMode(_enable?1:0);
	//	CMtpTarget::getInstance().controler().Camera.getMatrix()->getPos(ControlerFreeLookPos);
}

bool CEditorTask::enable()
{
	return _enable;
}

void CEditorTask::reset()
{
	_selectedElement = 0;
}

