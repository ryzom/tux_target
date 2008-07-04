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

#ifndef MT_ASYNC_JOB_H
#define MT_ASYNC_JOB_H


//
// Includes
//

#include <string>


//
// Functions
//

// download and gunzip a file in the current thread (not async)
void downloadFile(const std::string &src, const std::string &dst, bool quiet = false);
void decompressFile(const std::string &fn, bool tmp = false);


//
// Classes
//

struct CJob
{
	CJob() : Description("Unknown job"), Progression(0.0f) { }
	virtual void run() = 0;
	std::string Description; // you must not change this string during the run()
	volatile float Progression;	// value between 0 and 100 (100 = 100% = end)
};

class CAsyncRunnable;

class CAsyncJob : public NLMISC::CSingleton<CAsyncJob>
{
public:

	virtual void init();
	virtual void release();

	void test();

	void addJob(CJob *job);

	// download and gunzip a file in the async thread
	void addDownloadJob(const std::string &src, const std::string &dst, const std::string &useragent="");
	void addDecompressJob(const std::string &fn, bool tmp = false);

	bool downloadInQueue(const std::string &fn);

	std::string currentDescription();
	float currentProgression();
	
	bool empty() const;

	std::string toString();

private:

	CAsyncRunnable	*AsyncRunnable;
	NLMISC::IThread *AsyncThread;
};

#endif
