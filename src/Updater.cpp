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

#include "Updater.hpp"
	
extern "C"
{
	pid_t proc_find(const char* name) 
	{
		DIR* dir;
		struct dirent* ent;
		char* endptr;
		char buf[512];
		
		if (!(dir = opendir("/proc"))) {
			perror("can't open /proc");
			return -1;
		}

		while((ent = readdir(dir)) != NULL) {
			/* if endptr is not a null character, the directory is not
			 * entirely numeric, so ignore it */
			long lpid = strtol(ent->d_name, &endptr, 10);
			if (*endptr != '\0') {
				continue;
			}

			/* try to open the cmdline file */
			snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
			FILE* fp = fopen(buf, "r");
			
			if (fp) {
				if (fgets(buf, sizeof(buf), fp) != NULL) {
					/* check the first token in the file, the program name */
					char* first = strtok(buf, " ");
					size_t name_len = strlen(name); 
					size_t first_len = strlen(first);
					char* arr = new char[name_len];
					size_t i;
					for(i = 0; i < name_len; i++)
						arr[i] = first[first_len - name_len + i];
					if (!strcmp(arr, name)) {
						fclose(fp);
						closedir(dir);
						return (pid_t)lpid;
					}
					free(arr);
				}
				fclose(fp);
			}
			
		}
		
		closedir(dir);
		return -1;
	}
	
	int SIGNALRECEIVED = 0;
	void signalHandler(int signum)
	{
		if (signum == SIGUSR1)
		{
			SIGNALRECEIVED = 1;
		}
	}

	void registerHandler()
	{
		signal(SIGUSR1, signalHandler);
	}
}

Logger Updater::log_ = Logger();

/**
* @brief Ctor
*/
Updater::Updater(string confFile): confFile_(confFile)
{

}

/**
* @brief Dtor
*/
Updater::~Updater()
{
	
}

int Updater::update(string updateFile)
{
	//Initialize settings
	sanityTimeSec_ = 20;
	//log_.setOutputFile("up.log");
	parseConfig(confFile_);
	if(packageMap_.size() == 0)
		log_ << "ERROR: No Packages in file " << confFile_ << " found";
	// ### Register Signal handler for sanity check ###
	registerHandler();
	// ### Begin updateing ###
	unsigned found = updateFile.find_last_of("/\\");
    updatePath_ = updateFile.substr(0, found+1);
    updateFile_ = updateFile.substr(found + 1, updateFile.size() - 1);
	std::vector<Job> jobs;
	if(!parseUpdate(updateFile, jobs))
		return 0;
	int taskCount = 1;
	for(Job job : jobs)
	{
		log_ << "STATUS: Processing task " << taskCount << " of " << jobs.size() << endl; 
		if(!job.command_.compare("Remove"))
			doRemoveJob(job);
		if(!job.command_.compare("Update"))
			doUpdateJob(job);
		if(!job.command_.compare("Add"))
			doAddJob(job);
			// TODO remove package from map in case of failure
		taskCount++;
	}
	if(!writeConfig())
		return 0;
	return 1;
}

int Updater::parsePackage(ifstream& packageStream)
{
	Package new_pack;
	int check = 0;
	while(packageStream.good() && !new_pack.complete() && check < 7)
	{
		string line;
		std::getline(packageStream, line);
		std::string name_delim = "Name:";
		std::string version_delim = "Version:";
		std::string env1_delim = "Enviroment 1:";
		std::string env2_delim = "Enviroment 2:";
		std::string symlink_delim = "Symbolic Link:";
		std::string exec_delim = "Executable:";
		std::string sanity_delim = "Sanity Check:";
		size_t pos = 0;
		if((pos = line.find(name_delim)) != std::string::npos) 
			new_pack.name_ = trim(line.substr(pos + name_delim.length())), check++;
		else if((pos = line.find(version_delim)) != std::string::npos) 
			new_pack.version_ = trim(line.substr(pos + version_delim.length())), check++;
		else if((pos = line.find(env1_delim)) != std::string::npos) 
			new_pack.env1_ = trim(line.substr(pos + env1_delim.length())), check++;
		else if((pos = line.find(env2_delim)) != std::string::npos) 
			new_pack.env2_ = trim(line.substr(pos + env2_delim.length())), check++;
		else if((pos = line.find(symlink_delim)) != std::string::npos) 
			new_pack.symLink_ = trim(line.substr(pos + symlink_delim.length())), check++;
		else if((pos = line.find(exec_delim)) != std::string::npos) 
			new_pack.exec_ = trim(line.substr(pos + exec_delim.length())), check++;
		else if((pos = line.find(sanity_delim)) != std::string::npos) 
			new_pack.sanity_ = trim(line.substr(pos + sanity_delim.length())), check++;
		else if(line.size() != 0)
			log_ << "WARNING: Couldnt succsessfully parse line " << line <<  endl;
	}
	if(new_pack.complete() && check == 7)
	{
		auto retPair = packageMap_.insert(std::pair<string, Package>(new_pack.name_, new_pack));
		if(retPair.second)
			return 1;
		else
			return  log_ << "ERROR: Couldnt add Package " << new_pack.name_ << ", it already exists" << endl, 0;
	}
	else
		return log_ << "ERROR: Couldnt succsessfully parse Package" << endl, 0;
}

int Updater::parseAddPackage(ifstream& packageStream)
{
	Package new_pack;
	int check = 0;
	while(packageStream.good() && !new_pack.complete() && check < 7)
	{
		string line;
		std::getline(packageStream, line);
		std::string name_delim = "Name:";
		std::string version_delim = "Version:";
		std::string env1_delim = "Enviroment 1:";
		std::string env2_delim = "Enviroment 2:";
		std::string symlink_delim = "Symbolic Link:";
		std::string exec_delim = "Executable:";
		std::string sanity_delim = "Sanity Check:";
		size_t pos = 0;
		if((pos = line.find(name_delim)) != std::string::npos) 
			new_pack.name_ = trim(line.substr(pos + name_delim.length())), check++;
		else if((pos = line.find(version_delim)) != std::string::npos) 
			new_pack.version_ = trim(line.substr(pos + version_delim.length())), check++;
		else if((pos = line.find(env1_delim)) != std::string::npos) 
			new_pack.env1_ = trim(line.substr(pos + env1_delim.length())), check++;
		else if((pos = line.find(env2_delim)) != std::string::npos) 
			new_pack.env2_ = trim(line.substr(pos + env2_delim.length())), check++;
		else if((pos = line.find(symlink_delim)) != std::string::npos) 
			new_pack.symLink_ = trim(line.substr(pos + symlink_delim.length())), check++;
		else if((pos = line.find(exec_delim)) != std::string::npos) 
			new_pack.exec_ = trim(line.substr(pos + exec_delim.length())), check++;
		else if((pos = line.find(sanity_delim)) != std::string::npos) 
			new_pack.sanity_ = trim(line.substr(pos + sanity_delim.length())), check++;
		else if(line.size() != 0)
			log_ << "WARNING: Couldnt succsessfully parse line " << line <<  endl;
	}
	if(new_pack.complete() && check == 7)
	{
		auto retPair = addMap_.insert(std::pair<string, Package>(new_pack.name_, new_pack));
		if(retPair.second)
			return 1;
		else
			return  log_ << "ERROR: Couldnt add Package " << new_pack.name_ << ", it already exists" << endl, 0;
	}
	else
		return log_ << "ERROR: Couldnt succsessfully parse adding Package" << endl, 0;
}


string Updater::trim(string input)
{
	input.erase(
    std::remove_if(input.begin(), input.end(), 
	[](char c) -> bool
	{ 
		return std::isspace<char>(c, std::locale::classic()); 
	}), 
    input.end());
    return input;
}

int Updater::parseUpdateJob(ifstream& packageStream, Job& new_job)
{
	int check = 0;
	while(packageStream.good() && !new_job.complete())
	{
		string line;
		std::getline(packageStream, line);
		std::string name_delim = "Name:";
		std::string version_delim = "New Version:";
		std::string restart_delim = "Start/Restart:";
		std::string files_delim = "Path:";
		size_t pos = 0;
		if((pos = line.find(name_delim)) != std::string::npos) 
			new_job.name_ = trim(line.substr(pos + name_delim.length())), check++;
		else if((pos = line.find(version_delim)) != std::string::npos) 
			new_job.new_version_ = trim(line.substr(pos + version_delim.length())), check++;
		else if((pos = line.find(restart_delim)) != std::string::npos) 
			new_job.restart_ = trim(line.substr(pos + restart_delim.length())), check++;
		else if((pos = line.find(files_delim)) != std::string::npos) 
			new_job.path_ = trim(line.substr(pos + files_delim.length())), check++;
	}
	if(new_job.complete() && check >= 3)
		return 1;
	else
		return log_ << "ERROR: Couldnt succsessfully parse Package" << endl, 0;
	
}

int Updater::parseRemoveJob(ifstream& packageStream, Job& new_job)
{
	int check = 0;
	while(packageStream.good() && check < 2)
	{
		string line;
		std::getline(packageStream, line);
		std::string name_delim = "Name:";
		std::string hard_delim = "Hard:";
		size_t pos = 0;
		if((pos = line.find(name_delim)) != std::string::npos) 
			new_job.name_ = trim(line.substr(pos + name_delim.length())), check++;
		if((pos = line.find(hard_delim)) != std::string::npos) 
			new_job.hard_ = trim(line.substr(pos + hard_delim.length())), check++;
	}
	if(check == 2)
		return 1;
	else
		return log_ << "ERROR: Couldnt succsessfully parse Package" << endl, 0;
	
}

int Updater::parseConfig(string packageFile)
{
	log_ << "STATUS: Parse package file " << packageFile << endl;
	ifstream file;
	file.open(packageFile.c_str(), std::ifstream::in);
	if(file.good())
	{
		while(!file.eof())
		{
			string line;
			std::getline(file, line);
			std::string delimiter = "Package:";
			if(line.find(delimiter) != std::string::npos) 
				if(!parsePackage(file)) return 0;
		}
		return 1;
	}	
	else
		return log_ << "Error: No package file found "<< packageFile << std::endl, 0;
}

int Updater::parseUpdate(string updateFile, std::vector<Job>& jobs)
{
	log_ << "STATUS: Parsing job file " << updateFile << endl;
	ifstream file;
	file.open(updateFile.c_str(), std::ifstream::in);
	if(file.good())
	{
		while(!file.eof())
		{ 
			string line;
			std::getline(file, line);
			std::string UpDelimiter = "Update:";
			std::string AddDelimiter = "Add:";
			std::string RemDelimiter = "Remove:";
			if(line.find(UpDelimiter) != std::string::npos) 
			{
				Job new_job;
				new_job.command_ = "Update";
				if(!parseUpdateJob(file, new_job)) 
					return 0;
				jobs.push_back(new_job);
			}
			if(line.find(AddDelimiter) != std::string::npos) 
			{
				Job new_job;
				new_job.command_ = "Add";
				if(!parseAddPackage(file))
					return 0;
				auto revIt = addMap_.rbegin();
				new_job.name_ = revIt->second.name_;
			    jobs.push_back(new_job);
			}
			if(line.find(RemDelimiter) != std::string::npos) 
			{
				Job new_job;
				new_job.command_ = "Remove";
				if(!parseRemoveJob(file, new_job)) 
					return 0;
				else
					jobs.push_back(new_job);
			}
		}
		return 1;
	}	
	else
		return log_<<"Error: No package file found " << updateFile << std::endl, 0;
}

int Updater::writeConfig()
{
	ofstream file;
	file.open(confFile_.c_str());
	if(file.is_open())
	{
		for(auto it = packageMap_.begin(); it != packageMap_.end(); it++)
		{
			file << "Package: " << endl;
			file << "    Name: " << it->second.name_ << endl; 
			file << "    Version: " << it->second.version_ << endl; 
			file << "    Enviroment 1: " << it->second.env1_ << endl; 
			file << "    Enviroment 2: " << it->second.env2_ << endl; 
			file << "    Symbolic Link: " << it->second.symLink_ << endl; 
			file << "    Executable: " << it->second.exec_ << endl;
			file << "    Sanity Check: " << it->second.sanity_ << endl;
			file << endl;
		}
		return file.close(), 1;
	}	
	else
		return log_ << "Error: No package file found" << std::endl, 0;
}
int Updater::doAddJob(Job& addJob)
{
	log_ << "STATUS: Going to add package " << addJob.name_ << endl;
	auto packageIt = addMap_.find(addJob.name_);
	if(packageIt != addMap_.end())
	{
		packageMap_.insert(*packageIt);
		Package pack = packageIt->second;
		if(ub_.createDir(pack.env1_, 0755) < 0)
		{
			if(errno == EEXIST)
				return log_ << "ERROR: Failed to create environment 1 " << pack.env1_ << " from " << addJob.name_ << ", it already exists" << endl , 0;
			if(errno == EACCES)
				return log_ << "ERROR: Failed to create environment 1 " << pack.env1_ << " from " << addJob.name_ << ", due to the lack of permission" << endl , 0;
			// Some suspiscious error code
			return log_ << "ERROR: Failed to create environment 1 " << addJob.name_ << " Errno " << errno << endl , 0;
		}
		if(ub_.createDir(pack.env2_, 0755) < 0)
		{
			if(errno == EEXIST)
				return log_ << "ERROR: Failed to create environment 2 " << pack.env2_ << " from " << addJob.name_ << ", it already exists" << endl , 0;
			if(errno == EACCES)
				return log_ << "ERROR: Failed to create environment 2 " << pack.env2_ << " from " << addJob.name_ << ", due to the lack of permission" << endl , 0;
			// Some suspiscious error code
			return log_ << "ERROR: Failed to create environment 2 " << addJob.name_ << " Errno " << errno << endl , 0;
		}
		if(!(pack.env1_.back() == '/'))
				pack.env1_ = pack.env1_ + "/";
		if(!ub_.mvDir(updatePath_ + pack.name_ + "/", pack.env1_))
			return log_ << "ERROR: Failed to move new packages files" << endl, 0;
		int status = 0;
		if(fork() == 0)
		{ 
			log_ << "STATUS: Started executable" << endl;
			status = system(string(pack.env1_ + pack.exec_).c_str());
			exit(status);
		}
		if(!pack.sanity_.compare("true") || !pack.sanity_.compare("TRUE") || 
			!pack.sanity_.compare("True") || !pack.sanity_.compare("1") ||
			!pack.sanity_.compare("Yes") || !pack.sanity_.compare("yes"))
		{
			int time_count = 0;
			while(time_count < (sanityTimeSec_ * 1000) && !SIGNALRECEIVED)
			{
				usleep(1000);
				time_count++;
			}
			if(!SIGNALRECEIVED)
			{
				log_ << "ERROR: Sanity check failed with executable " << pack.exec_ << endl;
				return 0;
			}
			else
			{
				SIGNALRECEIVED = 0;
				log_ << "STATUS: Sanity check completed!" << endl;
			}
		}
		// ### Bend the symbolic link to updated environment ###
		log_ << "STATUS: Updating symbolic link " << pack.symLink_ << endl;
		int linkSuc = ub_.createSymbolicLink(pack.symLink_, pack.env1_ + pack.exec_);
		if(linkSuc != 0)
			return log_ << "ERROR: Couldnt create symbolic link " << pack.symLink_ << " error code " << linkSuc << endl, 0;
		return log_ << "STATUS: Succsessfully added Package " << addJob.name_ << endl, 1;
	}
	return 0;
		
}

int Updater::doRemoveJob(Job& rmJob)
{
	log_ << "STATUS: Going to remove package " << rmJob.name_ << endl;
	auto packageIt = packageMap_.find(rmJob.name_);
	if(packageIt != packageMap_.end())
	{
		Package pack = packageIt->second;
	
		if(!rmJob.hard_.compare("true") || !rmJob.hard_.compare("TRUE") || 
		   !rmJob.hard_.compare("True") || !rmJob.hard_.compare("1") ||
		   !rmJob.hard_.compare("Yes") || !rmJob.hard_.compare("yes"))
		{
			if((ub_.deleteDir(pack.env1_) != 1) || (ub_.deleteDir(pack.env2_) != 1))
				return log_ << "ERROR: Failed to remove " << rmJob.name_ << endl , 0;
			if(remove(pack.symLink_.c_str()))
				return log_ << "ERROR: Failed to remove symbolic link from " << rmJob.name_ << endl , 0;	
		}
		packageMap_.erase(rmJob.name_);
		return log_ << "STATUS: Package " << rmJob.name_ << " removed" << endl , 1;
	}
	else
		return log_ << "ERROR: Couldnt find package for removing: " << rmJob.name_ << endl, 0;
	
}		

int Updater::doUpdateJob(Job& upJob)
{
	log_ << "STATUS: Going to update package " << upJob.name_ << endl;
	auto packageIt = packageMap_.find(upJob.name_);
	if(packageIt != packageMap_.end())
	{
		Package pack = packageIt->second;
		if(atof(upJob.new_version_.c_str()) > atof(pack.version_.c_str()))
		{
			// ### Check the actually running environment ###
			string app_folder = ub_.getSymLink(pack.symLink_);
			if(!app_folder.compare(""))
				return log_ << "ERROR: Couldnt find symbolic link " << pack.symLink_ << endl, 0;
			string copy_to_path;
			string recover_path;
			// ### Set update path to the NOT running environment ###
			if(!app_folder.compare(pack.env1_))
			{
				copy_to_path = pack.env1_;
				recover_path = pack.env2_;
			}
			else
			{
				recover_path = pack.env1_;
				copy_to_path = pack.env2_;
			}
			if(!(copy_to_path.back() == '/'))
				copy_to_path = copy_to_path + "/";
			// ### Updating files ###			
			log_ << "STATUS: Updating environment " << copy_to_path << endl;
			if(upJob.path_.size() == 0)
				ub_.mvDir(updatePath_ + pack.name_ + "/", copy_to_path);
			else
				ub_.mvDir(upJob.path_, copy_to_path);
			string executable = copy_to_path + pack.exec_;
			// ### Kill actually running enviroment ###
			if(!upJob.restart_.compare("true") || !upJob.restart_.compare("TRUE") || 
				!upJob.restart_.compare("True") || !upJob.restart_.compare("1") ||
				!upJob.restart_.compare("Yes") || !upJob.restart_.compare("yes"))
			{
				int pid = proc_find(pack.exec_.c_str());
				if(pid > 0)
				{
					kill(pid, SIGTERM);
					log_ << "STATUS: Killed running process with PID: " << pid << endl;
				}
				else
					log_ << "WARNING: Couldnt find running process ... starting new one" << endl;
				int status = 0;
				if(fork() == 0)
				{ 
					log_ << "STATUS: Started executable" << endl;
					status = system(string(copy_to_path + pack.exec_).c_str());
					exit(status);
				}
				if(!pack.sanity_.compare("true") || !pack.sanity_.compare("TRUE") || 
				!pack.sanity_.compare("True") || !pack.sanity_.compare("1") ||
				!pack.sanity_.compare("Yes") || !pack.sanity_.compare("yes"))
				{
					int time_count = 0;
					while(time_count < (sanityTimeSec_ * 1000) && !SIGNALRECEIVED)
					{
						usleep(1000);
						time_count++;
					}
					if(!SIGNALRECEIVED)
					{
						log_ << "ERROR: Sanity check failed with executable " << pack.exec_ << endl;
						log_ << "STATUS: Starting recover version at environment: " << recover_path << endl;
						recoverExecutable(recover_path, pack);
						return 0;
					}
					else
						log_ << "STATUS: Sanity check completed!" << endl;
				}
			}
			// ### Bend the symbolic link to updated environment ###
			log_ << "STATUS: Updating symbolic link " << pack.symLink_ << endl;
			string tempLink = pack.symLink_ + "_updated";
			int linkSuc = ub_.createSymbolicLink(tempLink, executable);
			if(linkSuc != 0)
				return log_ << "ERROR: Couldnt create temporary symbolic link " << tempLink << " error code " << linkSuc << endl, 0;
			int writeSuc = ub_.mvFile(tempLink, pack.symLink_);
			if(writeSuc != 0)
				return log_ << "ERROR: Couldnt overwrite symbolic link " << pack.symLink_ << " error code " << writeSuc << endl, 0;			
			log_ << "STATUS: Succsessfully updated " << upJob.name_ << " from Version "
			<< packageIt->second.version_ << " to " << upJob.new_version_ << endl;
			packageIt->second.version_ = upJob.new_version_;
			return 1;
		}
		else
			return log_ << "WARNING: No update done version is already " << upJob.new_version_ << endl, 1;
	}
	else
		return log_ << "ERROR: Couldnt find package for updating: " << upJob.name_ << endl, 0;
}

int Updater::recoverExecutable(string recover_path, Package& package)
{
	if(!(recover_path.back() == '/'))
		recover_path = recover_path + "/";
	string recover_exec = recover_path + package.exec_;
	if(fork() == 0)
	{ 
		log_ << "STATUS: Starting recovery executable" << endl;
		int suc = system(string(recover_exec + " &").c_str());
		exit(suc);
	}
	return 1;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

