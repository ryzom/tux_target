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
#include "resource_manager2.h"

#include <curl/curl.h>

#ifdef MTPT_RESOURCE_MANAGER2

//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;

//
// Functions
//
int myProgressFunc(void *data, double t, double d, double ultotal, double ulnow)
{
	double pour1 = t!=0.0?d*100.0/t:0.0;
	double pour2 = ultotal!=0.0?ulnow*100.0/ultotal:0.0;
	//if(PatchThread)
	//	PatchThread->setState(false, false, "Getting file %s : %s / %s (%5.02f %%)", currentFile.c_str(), NLMISC::bytesToHumanReadable((uint32)d).c_str(), NLMISC::bytesToHumanReadable((uint32)t).c_str(), pour1);
	if(data)
	{
		float *f = (float *)data;
		*f = (float)pour1;
		//nlinfo(">> download percent = %f",pour1 );
	}
	return 0;
}

bool downloadFile (const string &source, const string &dest, void *data=NULL)
{
	nlinfo("downloadFileWithCurl file '%s' to '%s'", source.c_str(), dest.c_str());
	
	CURL *curl;
	CURLcode res;
	
	//setState(true, true, "Getting %s", NLMISC::CFile::getFilename (source).c_str ());
	//currentFile = NLMISC::CFile::getFilename (source);
	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	long r = 0;
	if(curl == NULL)
	{
		// file not found, delete local file
		nlwarning("curl init failed");
		return false;
		//throw Exception ("curl init failed");
	}
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, myProgressFunc);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, data);
	curl_easy_setopt(curl, CURLOPT_URL, source.c_str());
	
	// create the local file
	//setRWAccess(dest);
	FILE *fp = fopen (dest.c_str(), "wb");
	if (fp == NULL)
	{
		nlwarning("Can't open file '%s' for writing: (error code 37)", dest.c_str ());
		return false;
		//throw Exception ("Can't open file '%s' for writing: code=%d %s (error code 37)", dest.c_str (), errno, strerror(errno));
	}
	curl_easy_setopt(curl, CURLOPT_FILE, fp);
	
	//CurrentFilesToGet++;
	
	res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r);

	curl_easy_cleanup(curl);
	fclose(fp);
	
	curl_global_cleanup();
	
	//currentFile = "";
	
	if(CURLE_FTP_COULDNT_RETR_FILE == res)
	{
		// file not found, delete local file
		NLMISC::CFile::deleteFile(dest.c_str());
		nlwarning("curl download failed: (ec %d %d)", res, r);
		return false;
		//throw Exception ("curl download failed: (ec %d %d)", res, r);
	}
	
	if(CURLE_OK != res)
	{
		NLMISC::CFile::deleteFile(dest.c_str());
		nlwarning("curl download failed: (ec %d %d)", res, r);
		return false;
		//throw Exception ("curl download failed: (ec %d %d)", res, r);
	}
	
	if(r==404 || r==403)
	{
		// file not found, delete it
		NLMISC::CFile::deleteFile(dest.c_str());
		nlwarning("curl download failed: (ec %d %d)", res, r);
		return false;
		//throw Exception ("curl download failed: (ec %d %d)", res, r);
	}	
	nlinfo("downloadFileWithCurl done(r=%d).",r);
	return true;
}

void gunzipFile(string &filename)
{
	vector<uint8> buf;
	buf.resize(8000);
	uint8 *ptr = &(*(buf.begin()));

	string destfn = filename;
	string packedfn = filename+".gz";

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


//
// Thread
//

class CResourceManagerRunnable : public NLMISC::IRunnable
{
public:
	
	CResourceManagerRunnable()
	{
	}
	
	virtual void run()
	{
		DownloadList.clear();
		HttpServerFile = CConfigFileTask::getInstance().configFile().getVar("HttpServerFile").asString()+"/";
		CacheDirectory = CPath::standardizePath(CConfigFileTask::getInstance().configFile().getVar("CacheDirectory").asString());
		while(true)
		{
			string nextFilename = getNextFileToDownload();
			if(!nextFilename.empty())
			{
				startDownload(nextFilename);
				string res = getFile(nextFilename);
				downloadFinished(nextFilename);
			}
			nlSleep(10);
		}
	}

	
	void addFilename(string &filename)
	{
		if(filenameInQueue(filename)) return;
		CFairMutex m;
		m.enter();
		DownloadList.push_back(filename);
		m.leave();
	}

	bool filenameInQueue(string &filename)
	{
		bool res = false;
		CFairMutex m;
		m.enter();
		deque<string>::iterator it;
		for(it=DownloadList.begin();it!=DownloadList.end();it++)
		{
			if(*it==filename)
			{
				res = true;
				break;
			}
		}
		m.leave();
		return res;
	}

	bool getCurrent(string &filename,float &pos)
	{
		bool res = false;
		CFairMutex m;
		m.enter();
		res = !CurrentDownloadFilename.empty();
		filename = CurrentDownloadFilename;
		pos = CurrentDownloadPos;
		m.leave();
		return res;
	}
protected:
	friend int myProgressFunc(void *foo, double t, double d, double ultotal, double ulnow);	
	void setCurrentPos(float pos)
	{
		CFairMutex m;
		m.enter();
		CurrentDownloadPos = pos;
		m.leave();
	}

private:
	
	string getFile(string &filename)
	{
		string fn = filename;
		string srcfn = HttpServerFile + fn + ".gz";
		string destfn = CacheDirectory + fn;
		string packedfn = destfn + ".gz";
		string ext = CFile::getExtension(fn);
		bool res = downloadFile(srcfn,packedfn,&CurrentDownloadPos);
		if(!res && ext=="tga")
		{
			fn = CFile::getFilenameWithoutExtension(fn)+".dds";
			srcfn = HttpServerFile + fn + ".gz";
			destfn = CacheDirectory + fn;
			packedfn = destfn + ".gz";
			res = downloadFile(srcfn,packedfn,&CurrentDownloadPos);
		}
		if(res)
		{
			gunzipFile(destfn);
			CPath::addSearchFile(destfn);
			return destfn;
		}
		else
			return "";
	}
	
	
	void startDownload(string &filename)
	{
		CFairMutex m;
		m.enter();
		CurrentDownloadFilename = filename;
		CurrentDownloadPos = 0;
		m.leave();
	}

	void downloadFinished(string &filename)
	{
		CFairMutex m;
		m.enter();
		deque<string>::iterator it;
		for(it=DownloadList.begin();it!=DownloadList.end();it++)
		{
			if(*it==filename)
			{
				DownloadList.erase(it);
				break;
			}
		}
		CurrentDownloadFilename = "";
		CurrentDownloadPos = 0;
		m.leave();
	}

	string getNextFileToDownload()
	{
		string res = "";
		CFairMutex m;
		m.enter();
		if(DownloadList.size())
		{
			res = DownloadList.front();
		}
		m.leave();		
		return res;
	}

	deque<string> DownloadList;
	string CurrentDownloadFilename;
	float  CurrentDownloadPos;
	string HttpServerFile;
	string CacheDirectory;
};


//
// Functions
//

void CResourceManager::init()
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

	Connected = false;
	HttpServerFile = "";
	ResourceManagerRunnable = 0;
	ResourceManagerThread = 0;
}

void CResourceManager::loadChildren(const std::string &filename)
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


string CResourceManager::get(const string &filename)
{
	bool ok;
	return get(filename, ok);
}


void CResourceManager::update3DWhileDownloading()
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
		return;
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
	CBackgroundTask::getInstance().render();
	CChatTask::getInstance().render();
	CGuiTask::getInstance().render();
	
	
	//CFontManager::getInstance().littlePrintf(3.0f, 29.0f, str.c_str());
	
	CSwap3DTask::getInstance().render();
	
	//			mt3dSwap ();
	
}


string CResourceManager::get(const string &filename, bool &ok)
{
	string unk = "";
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

	
	string fn = strlwr(CFile::getFilename(filename).c_str());

	string path = CPath::lookup(fn, false, false);
	string fns = CFile::getFilename(path);

	double currentTime = CTimeTask::getInstance().time();
	float updatePercent = 0;
	guiSPG<CGuiFrame> mainFrame = 0;

	//nlinfo("CResourceManager get(%s)",filename.c_str());

	if(!connected())
	{
		return CResourceManagerLan::getInstance().get(filename,ok);
		if(!path.empty())
		{
			ok = true;
			return path;
		}
		else
			return unk;
	}
	else
	{
		nlinfo("chekcing '%s'",fn.c_str());
		CHashKey serverHashKey;
		filename2CRC::iterator it = CRCs.find(fn);
		if(!path.empty()) //prefer check against cached filename
		{
			filename2CRC::iterator it1 = CRCs.find(CFile::getFilename(path));
			if(it1!=CRCs.end())
				it = it1;
		}
		if(it!=CRCs.end())
		{
			nlinfo("key found in list");
			serverHashKey=it->second;
		}
		else
		{
			nlinfo("key NOT found in list(maybe remaped extension)");
		}
		
		if(!path.empty())
		{
			CHashKey hashKey = getSHA1(path);
			nlinfo("checking keys(local:server) : %s:%s",hashKey.toString().c_str(),serverHashKey.toString().c_str());
			if(hashKey==serverHashKey)
			{
				nlinfo("up to date");
				loadChildren(path);
				ok = true;
				return path;
			}
			else
				nlinfo("not up to date");
		}
		else
			nlinfo("file not in cache");
		nlinfo("downloading file");
		
		float updatePercent = 0;
		if(ResourceManagerRunnable)
		{
			ResourceManagerRunnable->addFilename(fn);
			guiSPG<CGuiXml> xml = CGuiXmlManager::getInstance().Load("updating.xml");
			mainFrame = (CGuiFrame *)xml->get("updatingFrame");
			guiSPG<CGuiText>  updatingMessage = (CGuiText *)xml->get("updatingMessage");
			updatingMessage->text = "Please wait while dowloading : ";
			guiSPG<CGuiText>  updatingFilename = (CGuiText *)xml->get("updatingFilename");
			updatingFilename->text = fn;
			guiSPG<CGuiProgressBar>  updatingProgressBar = (CGuiProgressBar *)xml->get("updatingProgressBar");
			updatingProgressBar->ptrValue(&updatePercent);
			CGuiObjectManager::getInstance().objects.push_back(mainFrame);		

			while(ResourceManagerRunnable && ResourceManagerRunnable->filenameInQueue(fn))
			{
				nlSleep(10);
				string filename = "";
				float pos = 0;
				ResourceManagerRunnable->getCurrent(filename,pos);
				updatePercent = pos;
				updatingFilename->text = filename;
				update3DWhileDownloading();
			}
			//keep alive the server
			CNetMessage msgout(CNetMessage::RequestCRCKey);
			msgout.serial(fn);
			msgout.serial(serverHashKey);
			CNetworkTask::getInstance().send(msgout);
		}
		string path = CPath::lookup(fn, false, false);
		if(path.empty())
		{
			if(!unk.empty()) //we can provide defautl file
			{
				nlinfo("return default filename : '%s'",unk.c_str());
				return unk;
			}
			else
			{
				nlwarning("cannot provide any file");
				string msg = toString("Cannot get file from server : '%s'\n please contact developers",fn.c_str());
				CMtpTarget::getInstance().error(msg);	
			}
		}
		nlinfo("return : '%s'",path.c_str());
		loadChildren(path);
		ok = true;
		return path;
	}
	
}

void CResourceManager::connected(bool c)
{
	if(Connected==c) return;

	if(c)
	{
		string crcFileName = CConfigFileTask::getInstance().configFile().getVar("CrcFileName").asString();
		HttpServerFile = CConfigFileTask::getInstance().configFile().getVar("HttpServerFile").asString()+"/";
		string srcfn = HttpServerFile + crcFileName;
		string destfn = CacheDirectory + crcFileName;
		bool res = downloadFile(srcfn,destfn);
		if(!res)
			return;


		FILE *fp = NULL;
		fp = fopen(destfn.c_str(),"rt");
		if(!fp)
			return;
			
		while(!feof(fp))
		{
			char *res = NULL;
			char filename[1024];
			res = fgets(filename,1024,fp);
			if(res==NULL || strlen(filename)==0) break;
			if(filename[strlen(filename)-1]=='\n')
				filename[strlen(filename)-1]='\0';

			char crcKey[1024];
			res = fgets(crcKey,1024,fp);
			if(res==NULL || strlen(crcKey)==0) break;
			if(crcKey[strlen(crcKey)-1]=='\n')
				crcKey[strlen(crcKey)-1]='\0';
			
			//nlinfo(">> adding %s:%s",filename,crcKey);

			CHashKey hashKey(crcKey);
			filename2CRC::iterator it = CRCs.find(filename);
			while(it!=CRCs.end())
			{
				CRCs.erase(it);		
				it = CRCs.find(filename);
			}
			CRCs.insert(filename2CRC::value_type(filename,hashKey));
		}
		fclose(fp);

		ResourceManagerRunnable = new CResourceManagerRunnable();
		nlassert(ResourceManagerRunnable);
		
		ResourceManagerThread = NLMISC::IThread::create(ResourceManagerRunnable);
		nlassert(ResourceManagerThread);
		
		ResourceManagerThread->start();
		
		Connected = true;
	}
	else
	{
		if(ResourceManagerThread && ResourceManagerRunnable)
		{
			ResourceManagerThread->terminate();
			delete ResourceManagerThread;
			ResourceManagerThread = 0;
			delete ResourceManagerRunnable;
			ResourceManagerRunnable = 0;
		}	
		Connected = false;
	}
}

bool CResourceManager::connected()
{
	return Connected;
}


string CResourceManager::getFile(string &filename,bool now)
{
	string fn = filename;
	string srcfn = HttpServerFile + fn;
	string destfn = CacheDirectory + fn;
	string ext = CFile::getExtension(fn);
	bool res = downloadFile(srcfn,destfn);
	if(!res && ext=="tga")
	{
		fn = CFile::getFilenameWithoutExtension(fn)+".dds";
		srcfn = HttpServerFile + fn;
		destfn = CacheDirectory + fn;
		res = downloadFile(srcfn,destfn);
	}
	if(res)
		return destfn;
	else
		return "";
}

#endif
