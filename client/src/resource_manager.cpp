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


#include <nel/misc/file.h>
#include <nel/misc/path.h>

#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/texture.h>
#include <nel/3d/material.h>
#include <nel/3d/shape_bank.h>
#include <nel/3d/driver_user.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>
#include <nel/3d/u_particle_system_instance.h>
#include <nel/3d/particle_system.h>
#include <nel/3d/particle_system_shape.h>

#include "zlib.h"

#include "main.h"
#include "3d_task.h"
#include "interface.h"
#include "time_task.h"
#include "chat_task.h"
#include "gui_task.h"
#include "mtp_target.h"
#include "task_manager.h"
#include "swap_3d_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "resource_manager.h"


//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Functions
//


void CResourceManagerLan::init()
{
	CPath::remapExtension("dds", "tga", true);
	CacheDirectory = CPath::standardizePath(CConfigFileTask::getInstance().configFile().getVar("CacheDirectory").asString());

	if(!CFile::isDirectory(CacheDirectory))
	{
		if(!CFile::createDirectory(CacheDirectory))
		{
			nlerror("Couldn't create \"%s\" directory", CacheDirectory.c_str());
		}
	}

	CConfigFile::CVar &v = CConfigFileTask::getInstance().configFile().getVar("Path");
	for (uint i = 0; i < v.size(); i++)
		CPath::addSearchPath (v.asString(i), true, false);

	CRCCheckTimes.clear();
}

void CResourceManagerLan::receivedCRC(string &fn)
{
	CRCReceived = true;
	CRCUpToDate = fn.empty();
	if(!CRCUpToDate)
	{
		nlinfo("CResourceManagerLan::receivedCRC %s: key different",fn.c_str());
		filename2LastCRCCheckTime::iterator it = CRCCheckTimes.find(CFile::getFilename(fn));
		if(it!=CRCCheckTimes.end())
			CRCCheckTimes.erase(it);		
	}
}


void CResourceManagerLan::receivedBlock(const string &res, const vector<uint8> &buf, bool eof, uint32 fileSize, bool receivedError)
{
	Reason = res;
	Buffer = buf;
	Eof = eof;
	Received = true;
	FileSize = fileSize;
	
	ReceivedError = receivedError;

	ReceivedFilename ="none";
	
	if(!ReceivedError)
	{
		nlinfo("Receive an answer of a download request block size %d eof %d", buf.size(), eof);
	}
	else
	{
		nlwarning("Error during download request '%s'", res.substr(strlen("ERROR:")).c_str());
		return;
	}
	ReceivedFilename = res.substr(strlen("FILE:"));
}

void CResourceManagerLan::loadChildren(const std::string &filename)
{
	string ext = CFile::getExtension(filename);
	if(ext == "shape")
	{
		// need to get texture inside the shape
		NL3D::registerSerial3d();

		CShapeStream ss;
		NLMISC::CIFile i(CPath::lookup(filename, false).c_str());
		i.serial(ss);
		i.close();

		CMesh *m = (CMesh*)ss.getShapePointer();
		uint nbm = m->getNbMaterial();
		for(uint i = 0; i < nbm; i++)
		{
			CMaterial &mat = m->getMaterial(i);
			for(uint j = 0; j < IDRV_MAT_MAXTEXTURES; j++)
			{
				ITexture *t = mat.getTexture(j);
				if(t)
				{
					CTextureFile *tf = dynamic_cast<CTextureFile *>(t);
					if(tf)
					{
						get(tf->getFileName());
					}
					else
					{
						CTextureMultiFile *tmf = dynamic_cast<CTextureMultiFile *>(t);
						if(tmf)
						{
							for(uint t = 0; t < tmf->getNumFileName(); t++)
								get(tmf->getFileName(t));
						}
					}
				}
			}
		}
	}
	else if(ext == "ps")
	{
		// need to get texture inside the shape
		NL3D::registerSerial3d();
		

		string fn = CFile::getFilename(filename);
		CShapeBank *bank = new CShapeBank;
		string shapeCache("mtptShapeCache");
		bank->addShapeCache(shapeCache);
		bank->setShapeCacheSize(shapeCache,1024*1024);
		std::vector<std::string> filelist;
		filelist.push_back(filename);
		CDriverUser *drv = (CDriverUser *)(&C3DTask::getInstance().driver());
		bank->preLoadShapes(shapeCache,filelist,string("*.ps"),NULL,true,drv->getDriver());
		bool b = bank->isShapeWaiting();
		IShape *is = bank->getShape(fn);
		//bank->load(filename)

		CParticleSystemShape *ps = (CParticleSystemShape *)is;
		
		uint numTexture = ps->getNumCachedTextures();
		nlinfo("loadchildren(%s) : num texture = %d",filename.c_str(),numTexture);
		
		for(uint i=0;i<numTexture;i++)
		{
			ITexture *tex = ps->getCachedTexture(i);
			CTextureFile *utex = (CTextureFile *)tex;
			nlinfo("loadchildren(%s) : texture = %s",filename.c_str(),utex->getFileName().c_str());
			get(utex->getFileName());
		}
		
		bank->reset();
		delete bank;
		
	}
}


bool CResourceManagerLan::waitNetworkMessage(bool stopFlag,bool &received, bool displayBackground)
{
		C3DTask::getInstance().update();
		CTimeTask::getInstance().update();
		//CBackgroundTask::getInstance().update();
		CChatTask::getInstance().update();
		CGuiTask::getInstance().update();
		CNetworkTask::getInstance().update();
		
		if (C3DTask::getInstance().kbPressed(KeyESCAPE) || !C3DTask::getInstance().driver().isActive() || !CNetworkTask::getInstance().connected())
		{
			exit(1);
			received = false;
			return false;
		}
		
		//			mt3dUpdate();
		//			mtpTarget::getInstance().updateTime ();
		//			mtpTarget::getInstance().Interface2d.updateChat();
		
		C3DTask::getInstance().clear();
		//			mt3dClear ();
		
		// ace todo put this in task	
		
		//			mtpTarget::getInstance().Interface2d.displayBackground();
		//			mtpTarget::getInstance().Interface2d.displayChat(true);
		
		C3DTask::getInstance().render();
		if(displayBackground)
			CBackgroundTask::getInstance().render();
		CChatTask::getInstance().render();
		CGuiTask::getInstance().render();
		
		//CFontManager::getInstance().littlePrintf(3.0f, 29.0f, str.c_str());
		
		CSwap3DTask::getInstance().render();
		
		//			mt3dSwap ();
		
		nlSleep(1);

		if(!stopFlag)
			received = true;
		return !stopFlag;
}


void CResourceManagerLan::refresh(const string &filename)
{
	string fn = CFile::getFilename(filename);
	filename2LastCRCCheckTime::iterator it = CRCCheckTimes.find(fn);
	while(it!=CRCCheckTimes.end())
	{
		CRCCheckTimes.erase(it);		
		it = CRCCheckTimes.find(fn);
	}
}

string CResourceManagerLan::get(const string &filename)
{
	bool ok;
	return get(filename, ok);
}

string CResourceManagerLan::get(const string &filename, bool &ok)
{
	string unk;
	ok = false;

	if(filename.empty())
		return unk;

	string ext = CFile::getExtension(filename);
	if(ext == "shape") unk = CPath::lookup("unknown.shape", false);
	//else if(ext == "lua") unk = CPath::lookup("unknown.lua", false);
	else if(ext == "tga") unk = CPath::lookup("unknown.tga", false);
	else if(ext == "dds") unk = CPath::lookup("unknown.tga", false);
	else if(ext == "png") unk = CPath::lookup("unknown.tga", false);
	else if(ext == "ps") unk = CPath::lookup("unknown.ps", false);
	else if(ext == "wav") unk = CPath::lookup("unkown.wav", false);
	else 
	{
		nlwarning("Extension not managed : %s(%s)",ext.c_str(),filename.c_str());
	}

	
	string fn = CFile::getFilename(filename);

	string path = CPath::lookup(fn, false, false);
	string fns = CFile::getFilename(path);

	double currentTime = CTimeTask::getInstance().time();
	float updatePercent = 0;
	guiSPG<CGuiFrame> mainFrame = 0;

	//nlinfo("CResourceManagerLan get(%s)",filename.c_str());

	if(!path.empty())
	{
		//nlinfo("!path.empty()");
		CRCUpToDate = true;
		if(CNetworkTask::getInstance().connected())
		{
			//nlinfo("connected");
			double lastCheckTime = 0;
			filename2LastCRCCheckTime::iterator it = CRCCheckTimes.find(fn);
			if(it!=CRCCheckTimes.end())
				lastCheckTime = (*it).second;
			if( it==CRCCheckTimes.end() && CConfigFileTask::getInstance().configFile().getVar("CRCCheck").asInt())
			{
				guiSPG<CGuiXml> xml = CGuiXmlManager::getInstance().Load("checking.xml");
				mainFrame = (CGuiFrame *)xml->get("checkingFrame");
				guiSPG<CGuiText>  checkingMessage = (CGuiText *)xml->get("checkingMessage");
				checkingMessage->text = "Please wait while checking : ";
				guiSPG<CGuiText>  checkingFilename = (CGuiText *)xml->get("checkingFilename");
				checkingFilename->text = fn;
				CGuiObjectManager::getInstance().objects.push_back(mainFrame);

				nlinfo("CResourceManagerLan::get(%s) sending RequestCRCKey and waiting result",fn.c_str());
				CHashKey hashKey = getSHA1(path);
				CNetMessage msgout(CNetMessage::RequestCRCKey);
				msgout.serial(fns);
				msgout.serial(hashKey);
				CNetworkTask::getInstance().send(msgout);
				
				CRCReceived = false;
				bool messageReceived;

				uint tid = getThreadId();
				nlassert(tid==TaskManagerThreadId || tid==NetworkThreadId);
				if(tid==TaskManagerThreadId)
				{
					while(waitNetworkMessage(CRCReceived,messageReceived) && !CMtpTarget::getInstance().error())
						;
				}
				else
					while(waitNetworkMessage(CRCReceived,messageReceived) && !CMtpTarget::getInstance().error());
					
				if(!messageReceived)
					return unk;
				if(it!=CRCCheckTimes.end())
					CRCCheckTimes.erase(it);
				CRCCheckTimes.insert(filename2LastCRCCheckTime::value_type(fn,currentTime));
			}
		}

		if(CRCUpToDate)
		{
			//nlinfo("CRCUpToDate");
			loadChildren(path);
			// we already have the file on local
			ok = true;
			return path;
		}
		else
		{
			nlinfo("CResourceManagerLan::get %s : key diferent",fn.c_str());			
		}
	}

	if(!CNetworkTask::getInstance().connected())
	{
		//nlinfo("!connected");
		// we can't download the file
		return unk;
	}
	
	guiSPG<CGuiXml> xml = CGuiXmlManager::getInstance().Load("updating.xml");
	mainFrame = (CGuiFrame *)xml->get("updatingFrame");
	guiSPG<CGuiText>  updatingMessage = (CGuiText *)xml->get("updatingMessage");
	updatingMessage->text = "Please wait while dowloading : ";
	guiSPG<CGuiText>  updatingFilename = (CGuiText *)xml->get("updatingFilename");
	updatingFilename->text = fn;
	guiSPG<CGuiProgressBar>  updatingProgressBar = (CGuiProgressBar *)xml->get("updatingProgressBar");
	updatingProgressBar->ptrValue(&updatePercent);
	CGuiObjectManager::getInstance().objects.push_back(mainFrame);		
	
	
	Eof = false;
	uint32 part = 0;
	FileSize = 0;
	
	string dlfn = CacheDirectory + fn;
	string packedfn = dlfn + ".gz";
	
	if(updatingMessage)
		updatingMessage->text = "Please wait while dowloading : ";

	if(CFile::fileExists(packedfn))
		CFile::deleteFile(packedfn);
	if(CFile::fileExists(dlfn))
		CFile::deleteFile(dlfn);
	
	while(!Eof)
	{
		if(CMtpTarget::getInstance().error())
		{
			CFile::deleteFile(packedfn);
			return unk;			
		}
		
		Received = false;
		CNetMessage msgout(CNetMessage::RequestDownload);
		msgout.serial(fn);
		msgout.serial(part);
		CNetworkTask::getInstance().send(msgout);

		string str = toString("Please wait while downloading '%s' part %d", fn.c_str(), part);

		bool messageReceived;
		uint tid = getThreadId();
		nlassert(tid==TaskManagerThreadId || tid==NetworkThreadId);
		if(tid==TaskManagerThreadId)
		{
			while(waitNetworkMessage(Received,messageReceived))
				;
		}
		else
			while(waitNetworkMessage(Received,messageReceived));
		if(!messageReceived)
		{
			CFile::deleteFile(packedfn);
			exit(1);
		}

		if(ReceivedError)
		{
			CFile::deleteFile(packedfn);
			return unk;
		}

		nlinfo("Receive an answer of a download request block size %s %d eof %d", Reason.c_str(), Buffer.size(), Eof);

		FILE *fp = fopen(packedfn.c_str(), "ab");
		if(!fp)
		{
			nlwarning("Couldn't open file '%s'", packedfn.c_str());
			return unk;
		}
		if(fwrite(&*Buffer.begin(), 1, Buffer.size(), fp) != Buffer.size())
		{
			nlwarning("Couldn't write file '%s'", packedfn.c_str());
			fclose(fp);
			return unk;
		}
		fclose(fp);

		part += Buffer.size();
		if(FileSize)
			updatePercent = ((float)part) / FileSize;
		if(part>FileSize)
		{
			nlwarning("CResourceManagerLan::get() received more data than expected");
			break;
		}
	}

	string destfn = CacheDirectory + ReceivedFilename;
	nlinfo("Received the whole file '%s'", destfn.c_str());
	if(CFile::fileExists(destfn))
		CFile::deleteFile(destfn);
		
		vector<uint8> buf;
		buf.resize(8000);
		uint8 *ptr = &(*(buf.begin()));
	{
		FILE *fp = fopen(destfn.c_str(), "wb");
		gzFile gzfp = gzopen(packedfn.c_str(), "rb");
		if(fp && gzfp)
		{
			while (!gzeof(gzfp)) 
			{
				uint32 res = gzread(gzfp, ptr, 8000);
				fwrite(ptr,1,res,fp);
			}
			fclose(fp);
			gzclose(gzfp);
		}
		CFile::deleteFile(packedfn);		
	}
	
	CPath::addSearchFile(destfn);

	loadChildren(destfn);
	CRCCheckTimes.insert(filename2LastCRCCheckTime::value_type(fn,currentTime));
				
	// need to download the file
	ok = true;
	return destfn;
}

