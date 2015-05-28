//## Copyright (c) 2015, Tristan Igelbrink
//## All rights reserved.
//## 
//## Redistribution and use in source and binary forms, with or without
//## modification, are permitted provided that the following conditions 
//## are met:
//##
//## 1. Redistributions of source code must retain the above copyright
//## notice, this list of conditions and the following disclaimer.
//##
//## 2. Redistributions in binary form must reproduce the above copyright
//## notice, this list of conditions and the following disclaimer in the
//## documentation and/or other materials provided with the distribution.
//## 
//## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//## COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT
//## , INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//## SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//## STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//## OF THE POSSIBILITY OF SUCH DAMAGE.

#include "EmSyRU.hpp"

Logger EmSyRU::log_ = Logger();

/**
* @brief Ctor
*/
EmSyRU::EmSyRU(string user, string dlURL, string pw, string env): 
                  dlURL_(dlURL), up_(env_ + "../" + packageFile_)
{
	unsigned found = dlURL.find_last_of("/\\");
	upURL_ = dlURL.substr(0, found+1);
    jobFile_ = dlURL.substr(found + 1, dlURL.size() - 1);
    log_ = Logger(logFile_);
    //log_ = Logger();
	ub_.setLogger(log_);
	curli_ = CurlCommunicator(user, pw, 10, 20);
	curli_.setLogger(log_);
	up_.setLogger(log_);
	if(!env.compare(""))
		env_ = ub_.getSymLink("/proc/self/exe");
	else
	   env_ = env;
	if(!(env_.back() == '/'))
		env_ += "/";
	env_ += "Workbench/";
}  

/**
* @brief Dtor
*/
EmSyRU::~EmSyRU()
{
	
}

int EmSyRU::update()
{
	// check for clean wokrspace
	if(!prepareWorkbench())
	{
		log_ << "ERROR: Failed to get in clean state, going to delete workbench " << endl;
		ub_.deleteDir(env_);
		if(!prepareWorkbench())
			return log_ << "ERROR: Failed create workbench" << endl ,0;
	}
    log_ << "STATUS: Searching for jobs at URL: " << dlURL_ << endl;
	int dlSuc = curli_.startDownload(dlURL_, env_ + jobFile_);
	if(dlSuc == 0)
	{
		log_ << "STATUS: No jobs to do" << endl;
		return 1;
	}
	if(dlSuc > 1)
	{
		log_ << "ERROR: Couldnt download job file" << endl;
		uploadLog();
		return 1;
	}
	log_ << "STATUS: Downloaded new job file" << endl;
    //char* filename = argv[1];
    //char* keyfilename = argv[2];
    //char* keyivfilename = argv[3];

	
	int tarSuc = system(string("tar -xf " + env_ + jobFile_ + " -C " + env_).c_str());
	if(tarSuc < 0)
	{
		log_ << "FATAL ERROR: Couldnt extract the decrypted files " << tarSuc << endl;
		uploadLog();
		return 1;
	}
	log_ << "STATUS: File extraction finished " << endl;
	
	// Decryption

	
	if(findJobConfFile())
	{
		if(up_.update(jobConfFile_))
			return 1;
		else
			return log_ << "ERROR: Failed completing update" << endl, 0;
	}
	log_ << "FATAL ERROR: Couldnt find " << jobConfFile_ << " at path " << env_ << endl;
	uploadLog();
	return 1;
	
}

void EmSyRU::uploadLog()
{
	log_ << "STATUS: Uploading logfile to " << upURL_ + logFile_ << endl;
	if(!curli_.startUpload(upURL_ + logFile_, logFile_))
		log_ << "ERROR: Failed to upload logfile " << endl;
	//ub_.deleteDir(env_);:
}

int EmSyRU::findJobConfFile()
{
	std::vector<string> files;
	if(!ub_.getFiles(env_, string(""), files))
	{
		log_ << "FATAL ERROR: Couldnt parse job files" << endl;
		return 0;
	}
	for(string f : files)
	{
		unsigned found = f.find_last_of("/\\");
		string jobConfFile = f.substr(found + 1, f.size() - 1);
		if(!jobConfFile.compare(jobConfFile_))
		{
			jobConfFile_ = string(env_ + jobConfFile);
			return 1;
		}
	} 
	return 0;
}

int EmSyRU::prepareWorkbench()
{
	if(ub_.createDir(env_, 0755) < 0)
	{
		if(errno == EACCES)
		{
			log_ << "ERROR: Failed to create workbench, due a the lack of permission" << endl;
			uploadLog();
			return 0;
		}
		if(errno == EEXIST)
		{
			log_ << "WARNING: No clean state going to process remaining tasks" << endl;
			if(findJobConfFile())
			{
				if(up_.update(jobConfFile_))
					return 1;
				else
					return log_ << "ERROR: Failed completing old update aborting" << endl, 0;
			}
			return log_ << "ERROR: No old Job file found ... but going ahead" << endl, 1;
		}
		if(errno != EEXIST)		
		{
			log_ << "ERROR: Failed to create workbench Errno " << errno << endl;
			uploadLog();
			return 0;
		}
	}
	return 1;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	

