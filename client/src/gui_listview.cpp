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
#include "time_task.h"
#include "gui_listview.h"
#include <nel/3d/u_material.h>
#include "gui_xml.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

int CGuiListView::getName(lua_State *luaSession)
{
	lua_pushstring(luaSession,name().c_str());
	return 1;
}

int CGuiListView::getSelected(lua_State *luaSession)
{
	uint selected = selectedRow() + 1;//we skip header
	if(rows.size()<=selected) return 0; //return nothing if nothing selectable

	list<guiSPG<CGuiHBox> >::iterator it;
	guiSPG<CGuiObject> item = 0;
	uint i=0;
	for(i=0,it=rows.begin();it!=rows.end();it++,i++)
	{
		if(i==selected)
		{
			item = *it;
			break;
		}
	}
	if(item)
	{
		item->luaPush(luaSession);
		return 1;
	}
	return 0;
}

int CGuiListView::pushBack(lua_State *luaSession)
{
	CGuiHBox *obj = (CGuiHBox *)Lunar<CGuiBox>::check(luaSession,1);
	rows.push_back(obj);
	return 0;
}

int CGuiListView::getCount(lua_State *luaSession)
{
	lua_Number count = rows.size();
	lua_pushnumber(luaSession,count);
	return 1;
}

int CGuiListView::getElement(lua_State *luaSession)
{
	lua_Number argIndex = luaL_checknumber(luaSession,1);
	uint index = (uint)argIndex;
	list<guiSPG<CGuiHBox> >::iterator it;
	guiSPG<CGuiObject> item = 0;
	uint i=0;
	for(i=0,it=rows.begin();it!=rows.end();it++,i++)
	{
		if(i==index)
		{
			item = *it;
			break;
		}
	}
	if(item)
	{
		item->luaPush(luaSession);
		return 1;
	}
	return 0;
}

int CGuiListView::removeElement(lua_State *luaSession)
{
	lua_Number argIndex = luaL_checknumber(luaSession,1);
	uint index = (uint)argIndex;

	list<guiSPG<CGuiHBox> >::iterator it;
	uint i=0;
	for(i=0,it=rows.begin();it!=rows.end();it++,i++)
	{
		if(i==index)
		{
			rows.remove(*it);
			break;
		}
	}
	return 0;
}






//
// Functions
//
	

void CGuiListViewManager::init()
{
	
	string res;

	res = CPath::lookup("row_listview.tga");
	_rowTexture = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_rowTexture);
	
	_rowTexture->setWrapS(UTexture::Clamp);
	
	
	_rowMaterial = C3DTask::instance().createMaterial();
	_rowMaterial.setTexture(_rowTexture);
	_rowMaterial.setBlend(true);
	_rowMaterial.setZFunc(UMaterial::always);
	_rowMaterial.setDoubleSided();
	
	res = CPath::lookup("selected_row_listview.tga");
	_selectedRowTexture = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_selectedRowTexture);
	
	_selectedRowTexture->setWrapS(UTexture::Clamp);
	
	
	_selectedRowMaterial = C3DTask::instance().createMaterial();
	_selectedRowMaterial.setTexture(_selectedRowTexture);
	_selectedRowMaterial.setBlend(true);
	_selectedRowMaterial.setZFunc(UMaterial::always);
	_selectedRowMaterial.setDoubleSided();
	
	res = CPath::lookup("header_listview.tga");
	_headerTexture = C3DTask::instance().driver().createTextureFile(res);
	nlassert(_headerTexture);
	
	_headerTexture->setWrapS(UTexture::Clamp);
	
	
	_headerMaterial = C3DTask::instance().createMaterial();
	_headerMaterial.setTexture(_headerTexture);
	_headerMaterial.setBlend(true);
	_headerMaterial.setZFunc(UMaterial::always);
	_headerMaterial.setDoubleSided();
	
	//CGuiHBox::xmlRegister();
	//CGuiVBox::xmlRegister();
	CGuiListView::xmlRegister();
	
}
	
void CGuiListViewManager::render()
{
}

void CGuiListViewManager::release()
{
	
}

NL3D::UTextureFile	*CGuiListViewManager::rowTexture()
{
	return _rowTexture;
}

NL3D::UMaterial CGuiListViewManager::rowMaterial()
{
	return _rowMaterial;
}

NL3D::UTextureFile	*CGuiListViewManager::selectedRowTexture()
{
	return _selectedRowTexture;
}

NL3D::UMaterial CGuiListViewManager::selectedRowMaterial()
{
	return _selectedRowMaterial;
}

NL3D::UTextureFile	*CGuiListViewManager::headerTexture()
{
	return _headerTexture;
}

NL3D::UMaterial CGuiListViewManager::headerMaterial()
{
	return _headerMaterial;
}



//
//
//
CGuiListView::CGuiListView()
{
	_spacing = 0;
	_selectedRow = 1;
	eventBehaviour = 0;
	quad.material(CGuiListViewManager::instance().rowMaterial());
}

CGuiListView::~CGuiListView()
{
	rows.clear();
}

float CGuiListView::spacing()
{
	return _spacing;
}

void CGuiListView::spacing(float spacing)
{
	_spacing = spacing;
}

CGuiObject::TGuiAlignment CGuiListView::alignment()
{
	CGuiObject::TGuiAlignment res = CGuiObject::alignment();
	list<guiSPG<CGuiHBox> >::iterator it;
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiObject *obj = *it;
		if(obj->alignment()&CGuiObject::eAlignExpandHorizontal)
			res = (TGuiAlignment )(res | eAlignExpandHorizontal);
		if(obj->alignment()&CGuiObject::eAlignExpandVertical)
			res = (TGuiAlignment )(res | eAlignExpandVertical);
	}
	
	return res;
}

void CGuiListView::alignment(int alignment)
{
	CGuiObject::alignment(alignment);
}


void CGuiListView::init(CGuiXml *xml,xmlNodePtr node)
{
	CGuiContainer::init(xml,node);
	
	node = xml->doc.getFirstChildNode(node,"element");
	if(node)
	{
		for( node = xml->doc.getFirstChildNode(node,"object");node;node = xml->doc.getNextChildNode(node,"object") )
		{
			CGuiObject *object = CGuiObject::XmlCreateFromNode(xml,node);
			if(object->getClassName()=="CGuiHBox")
			{
				rows.push_back((CGuiHBox *)object);
				xml->objects.push_back(object);
			}
			else
			{
				nlwarning("CGuiListView element not a CGuiHBox : %s(%s)",object->name().c_str(),object->getClassName().c_str());
				nlassert(false);
			}
		}
	}
}


void CGuiListView::_render(const CVector &pos,CVector &maxSize)
{
	H_AUTO2;
	uint rowCount = 0;
	bool _pressed = false;
	list<guiSPG<CGuiHBox> >::iterator it;
	if(rows.empty()) return;


	CVector globalPos = globalPosition(pos,maxSize);
	maxSize = expandSize(maxSize);
	
	deque<float> minWidths;
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiHBox *row = *it;
		deque<guiSPG<CGuiObject> >::iterator it2;
		uint rcount = 0;
		float mHeight = 0;
		for(it2=row->elements.begin();it2!=row->elements.end();it2++)
		{
			CGuiObject *obj = *it2;
			obj->position(CVector(5,0,0));

			if(obj->height()>mHeight)
				mHeight = obj->height();
			uint s = minWidths.size();
			if(minWidths.size()<=rcount)
				minWidths.push_back(0);

			if(minWidths[rcount]<obj->width())
				minWidths[rcount]=obj->width();
			rcount++;
		}
		row->minHeight(mHeight+10);
	}		
	
	float mWidth = 0;
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiHBox *row = *it;
		row->spacing(10);
		float rw = row->width();
		if(rw>mWidth)
			mWidth = rw;
	}	
	//rows[0]->width();
	
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiHBox *row = *it;
		//row->minWidth(mWidth);
		deque<guiSPG<CGuiObject> >::iterator it2;
		uint rcount = 0;
		for(it2=row->elements.begin();it2!=row->elements.end();it2++)
		{
			CGuiObject *obj = *it2;
			obj->alignment(CGuiObject::eAlignLeft|CGuiObject::eAlignCenterVertical);
			obj->minWidth(minWidths[rcount]);
			rcount++;
		}
	}	
	
	float expandableHeight = maxSize.y;
	bool expandableObjectPresent = false;
	float centerCount = 0;
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiObject *obj = *it;
		expandableHeight -= obj->height() + spacing();
		if(obj->alignment()&CGuiObject::eAlignExpandVertical)
			expandableObjectPresent = true;
		if(obj->alignment()&CGuiObject::eAlignCenterVertical)
			centerCount++;
	}
	expandableHeight += spacing();
	float centerSpacing = 0;
	if(!expandableObjectPresent && centerCount>0)
		centerSpacing = expandableHeight / centerCount;
	
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiHBox *row = *it;
		float objHeight = row->height();
		CVector newMaxSize(maxSize.x,0,0);
		if(expandableObjectPresent)
		{
			if(row->alignment()&CGuiObject::eAlignExpandVertical)
				newMaxSize.y = expandableHeight + objHeight;
			else
				newMaxSize.y = objHeight;
		}
		else
		{
			if(row->alignment()&CGuiObject::eAlignCenterVertical)
				newMaxSize.y = objHeight + centerSpacing;
			else
				newMaxSize.y = objHeight;
			if(row->alignment()&CGuiObject::eAlignBottom)
				globalPos.y += expandableHeight;
		}
		
		CVector oldMaxSize = newMaxSize;
		
		row->render(globalPos,newMaxSize);

		uint s = minWidths.size();
		float lx = 0;
		deque<guiSPG<CGuiObject> >::iterator it2;
		uint rcount = 0;
		for(it2=row->elements.begin();it2!=row->elements.end();it2++)
		{
			CGuiObject *obj = *it2;
			CGuiStretchedQuad quad;
			if(rowCount==0)
				quad.material(CGuiListViewManager::instance().headerMaterial());
			else
			{
				if(rowCount==_selectedRow)
					quad.material(CGuiListViewManager::instance().selectedRowMaterial());
				else
					quad.material(CGuiListViewManager::instance().rowMaterial());
			}
			//		quad.size(CVector(obj->width(),obj->height()-0,0));
			CVector qSize= obj->renderedSize();
			if(rcount<row->elements.size()-1)
				qSize += CVector(row->spacing(),0,0);
			//qSize += CVector(0,0,0);
			qSize.y = row->height();
			quad.size(qSize);
			quad.position(obj->renderedPos());
			quad.render();
			rcount++;
		}

		CVector mousePos = CGuiObjectManager::instance().mouseListener().position();
		
		if(row->isIn(mousePos,row->renderedPos(),row->renderedSize()))
		{
			CVector mousePressedPos = CGuiObjectManager::instance().mouseListener().pressedPosition();
			
			if(CGuiObjectManager::instance().mouseListener().ButtonDown)
			{
				if(row->isIn(mousePressedPos,row->renderedPos(),row->renderedSize()))
					_selectedRow = rowCount;
				else
					_selectedRow = 0;
			}
			if(CGuiObjectManager::instance().mouseListener().DoubleClicked)
			{
				if(eventBehaviour)
					eventBehaviour->onPressed(_selectedRow-1);
				//nlinfo("CGuiListView double click : %d",_selectedRow-1);				
			}
		}
		
		
		
		if(row->alignment()&CGuiObject::eAlignCenterVertical)
			newMaxSize.y = objHeight + centerSpacing;
		globalPos.y += newMaxSize.y + spacing();
		expandableHeight -= newMaxSize.y - objHeight; 
		if(expandableHeight<0)
			nlwarning("expandableHeight(%f)<0",expandableHeight);
		rowCount++;
	}
}

float CGuiListView::_width()
{
	list<guiSPG<CGuiHBox> >::iterator it;
	float res = 0;
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiHBox *row = *it;
		float rowWidth = row->width();
		uint rowElementCount = row->elements.size();
		/*
		if(rowElementCount>0)
			rowWidth += (rowElementCount) * 5;
			*/
		if(res<rowWidth)
			res = rowWidth;
	}	
	return res;
}

float CGuiListView::_height()
{
	list<guiSPG<CGuiHBox> >::iterator it;
	float res = 0;
	for(it=rows.begin();it!=rows.end();it++)
	{
		CGuiObject *obj = *it;
		res += obj->height();
	}	
	if(rows.size()>1)
		return res + (rows.size()-1) * spacing();	
	else
		return res;
}

void CGuiListView::luaPush(lua_State *L)
{
	Lunar<CGuiListView>::push(L, this);
}


void CGuiListView::xmlRegister()
{
	CGuiObjectManager::instance().registerClass("CGuiListView",CGuiListView::create);
}

CGuiObject *CGuiListView::create()
{
	CGuiObject *res = new CGuiListView;
	
	return res;	
}


//
//
//

CGuiListViewEventBehaviour::CGuiListViewEventBehaviour()
{
}

CGuiListViewEventBehaviour::~CGuiListViewEventBehaviour()
{
}

void CGuiListViewEventBehaviour::onPressed(int rowId)
{
}
