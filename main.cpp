#include "Updater.hpp"
#include "Decrypter.h"
#include "CurlCommunicator.hpp"
#include "Logger.hpp"
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>

using std::string;


/*std::string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}*/

CurlCommunicator curli;
Logger log_;
string logFile = "job.log";
string dlURL = "ftp://ftp1.5hosting.com/job.tar";
string upURL = "ftp://ftp1.5hosting.com/";

void uploadLog()
{
	log_ << "Finished: Uploading logfile to " << upURL << endl;
	if(!curli.startUpload(upURL + logFile, logFile))
		log_ << "ERROR: Failed to upload logfile " << endl;
}


int main(int argc, char **argv) {
	log_ = Logger(logFile);
	/*if(argc != 4) {
        fprintf(stderr, "To few file arguments supplied.\n");
        return 1;
    }*/
    
    log_ << "STATUS: Searching for jobs at URL: " << dlURL << endl;
	curli = CurlCommunicator(10, 20);
	curli.setLogger(log_);
	int dlSuc = curli.startDownload(dlURL, "job.tar" );
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
	Updater up("../Packages.conf");
	up.setLogger(log_);
	string env = "/home/tristan/test2/remote_update/build/";
    /*char* filename = argv[1];
    char* keyfilename = argv[2];
    char* keyivfilename = argv[3];
    int length = 0;
	if(decryptFile(filename, keyfilename, keyivfilename, length) != 1)
		return up.log_ << "FATAL ERROR: Couldnt decrypt the update " << string(filename) << endl, 1;
	up.log_ << "STATUS: Decrypted succsessfully the update " << string(filename) << " with " << length << " bytes " << endl;
	string dec_file_name = string(filename) + ".dec";
	string dec_dir_name = string(filename) + "_dec";*/
	string job_dir = string("Workbench");
	if(up.createDir(job_dir, 0755) < 0)
	{
		if(errno == EACCES)
		{
			log_ << "ERROR: Failed to create workbench, due a the lack of permission" << endl;
			uploadLog();
			return 1;
		}
		if(errno != EEXIST)		
		{
			log_ << "ERROR: Failed to create workbench Errno " << errno << endl;
			uploadLog();
			return 1;
		}
	}
	int tarSuc = system(string("tar -xf job.tar -C " + job_dir).c_str());
	if(tarSuc < 0)
	{
		log_ << "FATAL ERROR: Couldnt extract the decrypted files " << tarSuc << endl;
		uploadLog();
		return 1;
	}
	log_ << "STATUS: File extraction finished " << endl;
	std::vector<string> files;
	string upPath = string(env) + job_dir + "/";
	if(!up.getUpdateFiles(upPath, string(""), files))
	{
		log_ << "FATAL ERROR: Couldnt parse job files" << endl;
		uploadLog();
		return 1;
	}
	for(string f : files)
	{
		unsigned found = f.find_last_of("/\\");
		string updateFile = f.substr(found + 1, f.size() - 1);
		if(!updateFile.compare("Job.conf"))
		{
			up.update(env + job_dir + "/Job.conf");
			uploadLog();
			return 1;
		}
	} 
	uploadLog();
	log_ << "FATAL ERROR: Couldnt find Job.conf " << endl;
	return 1;
}
