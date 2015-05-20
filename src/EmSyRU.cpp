#include "EmSyRU.hpp"
#include "Crypto.h"
#include "Decrypter.h"

Logger EmSyRU::log_ = Logger();

/**
* @brief Ctor
*/
EmSyRU::EmSyRU(string dlURL, string upURL, string env): 
                  dlURL_(dlURL), upURL_(upURL), env_(env), up_("../" + packageFile_)
{
	unsigned found = dlURL.find_last_of("/\\");
    jobFile_ = dlURL.substr(found + 1, dlURL.size() - 1);
    log_ = Logger(logFile_);
	ub_.setLogger(log_);
	curli_ = CurlCommunicator(10, 20);
	curli_.setLogger(log_);
	up_.setLogger(log_);
}

/**
* @brief Dtor
*/
EmSyRU::~EmSyRU()
{
	
}

int EmSyRU::update()
{
    log_ << "STATUS: Searching for jobs at URL: " << dlURL_ << endl;
	int dlSuc = curli_.startDownload(dlURL_, jobFile_ );
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
    /*char* filename = argv[1];
    char* keyfilename = argv[2];
    char* keyivfilename = argv[3];
    int length = 0;
	if(decryptFile(filename, keyfilename, keyivfilename, length) != 1)
		return up.log_ << "FATAL ERROR: Couldnt decrypt the update " << string(filename) << endl, 1;
	up.log_ << "STATUS: Decrypted succsessfully the update " << string(filename) << " with " << length << " bytes " << endl;
	string dec_file_name = string(filename) + ".dec";
	string dec_dir_name = string(filename) + "_dec";*/
	
	int tarSuc = system(string("tar -xf job.tar -C " + jobDir_).c_str());
	if(tarSuc < 0)
	{
		log_ << "FATAL ERROR: Couldnt extract the decrypted files " << tarSuc << endl;
		uploadLog();
		return 1;
	}
	log_ << "STATUS: File extraction finished " << endl;
	std::vector<string> files;
	string upPath = string(env_ + jobDir_ + "/");
	if(!ub_.getFiles(upPath, string(""), files))
	{
		log_ << "FATAL ERROR: Couldnt parse job files" << endl;
		uploadLog();
		return 1;
	}
	for(string f : files)
	{
		unsigned found = f.find_last_of("/\\");
		string updateFile = f.substr(found + 1, f.size() - 1);
		if(!updateFile.compare(jobFile_))
		{
			up_.update(string(env_ + jobDir_ + "/" + jobFile_));
			uploadLog();
			return 1;
		}
	} 
	uploadLog();
	log_ << "FATAL ERROR: Couldnt find Job.conf " << endl;
	return 1;
	
}

void EmSyRU::uploadLog()
{
	log_ << "Finished: Uploading logfile to " << upURL_ << endl;
	if(!curli_.startUpload(upURL_ + logFile_, logFile_))
		log_ << "ERROR: Failed to upload logfile " << endl;
}

int EmSyRU::findJobFile(string upPath, string& jobfile)
{
	std::vector<string> files;
	if(!ub_.getFiles(upPath, string(""), files))
	{
		log_ << "FATAL ERROR: Couldnt parse job files" << endl;
		return 0;
	}
	for(string f : files)
	{
		unsigned found = f.find_last_of("/\\");
		jobfile = f.substr(found + 1, f.size() - 1);
		if(!jobfile.compare(jobFile_))
		{
			jobfile = string(upPath + "/" + jobFile_);
			return 1;
		}
	} 
	return 0;
}

int EmSyRU::prepareWorkbench()
{
	if(ub_.createDir(jobDir_, 0755) < 0)
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
			string jobfile;
			if(findJobFile(env_ + jobDir_, jobFile_))
			{
				if(up_.update(env_ + jobDir_ + "/" + jobFile_))
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
	
	
	
	
	
	
	
	
	
	
	
	
	
	

