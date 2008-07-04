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

#include <deque>
#include <zlib.h>
#include <curl/curl.h>

#include <nel/misc/md5.h>
#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/misc/command.h>

#include "async_job.h"


//
// Namespaces
//

using namespace NLMISC;


//
// Functions
//
	
class CAsyncRunnable : public NLMISC::IRunnable
{
public:
	
	CAsyncRunnable() : CurrentJob(0) { }
	
	virtual void run()
	{
		//nlinfo("Starting Async thread");
		while(true)
		{
			//if(!JobQueue.empty()) nlinfo("New job, process them");
			while(!JobQueue.empty())
			{
				Mutex.enter();
				CurrentJob = JobQueue.front();
				JobQueue.pop_front();
				Mutex.leave();
				//nlinfo("Processing the job %p", CurrentJob);
				CurrentJob->run();
				//nlinfo("Destroying the job %p", CurrentJob);
				CJob *j = CurrentJob;
				CurrentJob = 0;
				delete j;
				//if(JobQueue.empty()) nlinfo("No Job left, go in sleep mode");
			}
			nlSleep(100);
		}
		//nlinfo("Ending Async thread");
	}

private:

	// You must lock this mutex when trying to access any variables here
	NLMISC::CFastMutex Mutex;

	std::deque<CJob*> JobQueue;

	CJob *CurrentJob;

	friend class CAsyncJob;
};

void CAsyncJob::init()
{
	AsyncRunnable = new CAsyncRunnable();
	nlassert(AsyncRunnable);

	AsyncThread = NLMISC::IThread::create(AsyncRunnable);
	nlassert(AsyncThread);

	AsyncThread->start();
}

void CAsyncJob::release()
{
	if(AsyncThread)
	{
		AsyncThread->terminate();
		delete AsyncThread;
		AsyncThread = 0;
	}
	if(AsyncRunnable)
	{
		delete AsyncRunnable;
		AsyncRunnable = 0;
	}
}

void CAsyncJob::addJob(CJob *job)
{
	nlassert(job);
	nlassert(AsyncRunnable);

	AsyncRunnable->Mutex.enter();
	AsyncRunnable->JobQueue.push_back(job);
	AsyncRunnable->Mutex.leave();
}

string CAsyncJob::currentDescription()
{
	nlassert(AsyncRunnable);

	string str;
	AsyncRunnable->Mutex.enter();
	if(AsyncRunnable->CurrentJob)
	{
		str = AsyncRunnable->CurrentJob->Description;
	}
	else
	{
		str = "No job";
	}
	AsyncRunnable->Mutex.leave();
	return str;
}

float CAsyncJob::currentProgression()
{
	nlassert(AsyncRunnable);

	float p = 0.0f;
	AsyncRunnable->Mutex.enter();
	if(AsyncRunnable->CurrentJob)
	{
		p = AsyncRunnable->CurrentJob->Progression;
	}
	AsyncRunnable->Mutex.leave();
	return p;
}

bool CAsyncJob::empty() const
{
	if(AsyncRunnable == 0)
		return true;
	bool res;
	AsyncRunnable->Mutex.enter();
	res = AsyncRunnable->CurrentJob==0 && AsyncRunnable->JobQueue.empty();
	AsyncRunnable->Mutex.leave();
	return res;
}

string CAsyncJob::toString()
{
	nlassert(AsyncRunnable);

	string str;
	AsyncRunnable->Mutex.enter();
	str = ::toString("%u JobsInQueue", AsyncRunnable->JobQueue.size());
	AsyncRunnable->Mutex.leave();
	str += ::toString(" %.2fpc", currentProgression());
	str += " '"+currentDescription()+"'";
	return str;
}

bool CAsyncJob::downloadInQueue(const std::string &fn)
{
	AsyncRunnable->Mutex.enter();
	bool iq = false;
	for(std::deque<CJob*>::iterator it = AsyncRunnable->JobQueue.begin(); it != AsyncRunnable->JobQueue.end(); it++)
	{
		if((*it)->Description.find(fn) != string::npos)
		{
			iq = true;
			break;
		}
	}
	if(!iq && AsyncRunnable->CurrentJob != 0 && AsyncRunnable->CurrentJob->Description.find(fn) != string::npos)
	{
		iq = true;
	}
	AsyncRunnable->Mutex.leave();
	return iq;
}


//
// Decompress Jobs
//

struct CDecompressJob : public CJob
{
	CDecompressJob(const string &fn, bool tmp = false) : Filename(fn), Temp(tmp)
	{
		Description = "Decompressing the file "+CFile::getFilename(Filename);
	}

	virtual void run()
	{
		nlinfo("Decompressing file '%s' tmp %s", Filename.c_str(), (Temp?"true":"false"));

		vector<uint8> buf;
		buf.resize(8000);
		uint8 *ptr = &(*(buf.begin()));

		string destfn = Filename;
		if(Temp)
			destfn += ".tmp";
		string packedfn = Filename+".gz";

		uint32 cur = 0, fs = CFile::getFileSize(packedfn);
		string fn = CFile::getFilename(Filename);

		FILE *fp = fopen(destfn.c_str(), "wb");
		if (fp == 0)
		{
			nlerror("Can't open file '%s' for writing", destfn.c_str());
			return;
		}
		gzFile gzfp = gzopen(packedfn.c_str(), "rb");
		if (gzfp == 0)
		{
			nlerror("Can't open file '%s' for reading", packedfn.c_str());
			return;
		}
		while (!gzeof(gzfp)) 
		{
			uint32 res = gzread(gzfp, ptr, 8000);
			fwrite(ptr, 1, res, fp);
			cur += res;
			Progression = float(cur)/float(fs);
		}
		fclose(fp);
		gzclose(gzfp);
		CFile::deleteFile(packedfn);		
	}

	string Filename;
	bool Temp;
};


//
// Download Jobs
//

struct CDownloadJob : public CJob
{
	static int myProgressFunc(void *data, double t, double d, double ultotal, double ulnow)
	{
		double pour1 = t!=0.0?d*100.0/t:0.0;
		double pour2 = ultotal!=0.0?ulnow*100.0/ultotal:0.0;
		CJob *j = (CJob *)data;
		j->Progression = (float)pour1;
		return 0;
	}

	CDownloadJob(const string &src, const string &dst, bool quiet = false, const std::string &useragent="") : Source(src), Destination(dst), Quiet(quiet), UserAgent(useragent)
	{
		Description = "Downloading the file "+CFile::getFilename(dst);
	}

	virtual void run()
	{
/*		if(!Quiet)
			nlinfo("Downloading file '%s' to '%s'", Source.c_str(), Destination.c_str());*/

		if(CFile::fileExists(Destination))
			CFile::deleteFile(Destination);

		curl_global_init(CURL_GLOBAL_ALL);
		CURL *curl = curl_easy_init();
		if(curl == 0)
		{
			nlerror("curl init failed");
			return;
		}
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, myProgressFunc);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(curl, CURLOPT_URL, Source.c_str());
		if(!UserAgent.empty())
			curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent.c_str());

		// create the local file
		FILE *fp = fopen (Destination.c_str(), "wb");
		if (fp == 0)
		{
			nlerror("Can't open file '%s' for writing", Destination.c_str());
			return;
		}
		curl_easy_setopt(curl, CURLOPT_FILE, fp);

		CURLcode Result = curl_easy_perform(curl);
		long ResponseCode = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ResponseCode);

		curl_easy_cleanup(curl);
		fclose(fp);

		curl_global_cleanup();

		if(CURLE_FTP_COULDNT_RETR_FILE == Result)
		{
			// file not found, delete local file
			NLMISC::CFile::deleteFile(Destination.c_str());
#ifdef NL_OS_WINDOWS
			MessageBoxA(NULL, "Mtp Target web site seems to be down. Go to the home page and if it's the case, please come back in a few minutes", toString("Mtp Target Error (ec %d %d)", Result, ResponseCode).c_str(), MB_OK);
#else
			nlerror("Mtp Target web site seems to be down. Go to the home page and if it's the case, please come back in a few minutes (ec %d %d)", Result, ResponseCode);
#endif
			exit(EXIT_FAILURE);
			return;
		}

		if(CURLE_OK != Result)
		{
			NLMISC::CFile::deleteFile(Destination.c_str());
#ifdef NL_OS_WINDOWS
			MessageBoxA(NULL, "Mtp Target web site seems to be down. Go to the home page and if it's the case, please come back in a few minutes", toString("Mtp Target Error (ec %d %d)", Result, ResponseCode).c_str(), MB_OK);
#else
			nlerror("Mtp Target web site seems to be down. Go to the home page and if it's the case, please come back in a few minutes (ec %d %d)", Result, ResponseCode);
#endif
			exit(EXIT_FAILURE);
			return;
		}

		if(ResponseCode==404 || ResponseCode==403)
		{
			// file not found, delete it
			NLMISC::CFile::deleteFile(Destination.c_str());
			nlwarning("Curl download failed: (ec %d %d) src: '%s' dest: '%s'", Result, ResponseCode, Source.c_str(), Destination.c_str());
			return;
		}
		//nlinfo("Downloading done (%d %d)", Result, ResponseCode);
	}

	string Source, Destination, UserAgent;
	bool Quiet;
};

void downloadFile (const string &src, const string &dst, bool quiet)
{
	CDownloadJob job(src, dst, quiet);
	job.run();
}

void decompressFile(const string &fn, bool tmp)
{
	CDecompressJob job(fn, tmp);
	job.run();
}

void CAsyncJob::addDownloadJob(const string &src, const string &dst, const string &useragent)
{
	CAsyncJob::instance().addJob(new CDownloadJob(src, dst, false, useragent));
}

void CAsyncJob::addDecompressJob(const string &fn, bool tmp)
{
	CAsyncJob::instance().addJob(new CDecompressJob(fn, tmp));
}


//
// Test
//

struct CTestJob : public CJob
{
	CTestJob(string &n) : Name(n)
	{
		Description = "Sleep 10 second with "+n;
	}
	virtual void run()
	{
		nlinfo("Hello %s, let sleep 10s", Name.c_str());
		for(uint i = 0; i < 100; i++)
		{
			Progression = float(i);
			nlSleep(100);
		}
		nlinfo("Good bye %s", Name.c_str());
	}
	string Name;
};

void CAsyncJob::test()
{
	//addJob(new CTestJob(string("bob")));
	//addJob(new CTestJob(string("john")));
	//addJob(new CTestJob(string("anna")));
	//string PatchServer = "http://mtp.jolt.co.uk/patch/1/";
	//addJob(new CDownloadJob(PatchServer + "patch_list.gz", "patch_list.gz"));
}

/*NLMISC_COMMAND(addJob1, "addJob1", "<name>")
{
	if(args.size() != 1) return false;
	CAsyncJob::instance().addJob(new CTestJob(string(args[0])));
	return true;
}

NLMISC_COMMAND(addJob2, "addJob2", "<name>")
{
	if(args.size() != 1) return false;
	string PatchServer = "http://mtp.jolt.co.uk/patch/1/";
	CAsyncJob::instance().addJob(new CDownloadJob(PatchServer + args[0] + ".gz", CFile::getFilename(args[0]) + ".gz"));
	return true;
}

NLMISC_COMMAND(addJob3, "addJob3", "<name>")
{
	if(args.size() != 1) return false;
	CAsyncJob::instance().addJob(new CDecompressJob(args[0]));
	return true;
}
*/
