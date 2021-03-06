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

/**
* @file Updater.hpp
* @brief Package class for holding and controling packages to update
* @author Tristan Igelbrink
* @date 20.03.2015
*/

#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <map>
#include <fstream>
#include <regex>
#include <dirent.h>
#include "Timestamp.hpp"
#include "Logger.hpp"
#include <fts.h>
#include <sys/stat.h>
#include "UnixBridge.hpp"


using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::regex;

/**
 * @brief   Pacakge class with all relevant package informations
 */
class Updater
{

public:
    /**
    * @brief Ctor
    */
    Updater(string confFile);
    

    /**
    * @brief Dtor
    */
    ~Updater();

	int update(string updateFile);
	//int getUpdateFiles(string dir, string upFile, std::vector<string>& files);
	//int createDir(string dir, mode_t mode);
	
	static Logger log_; /**< Logger for tracking and generating report to upload */
	static void setLogger(Logger& log){log_ = log; UnixBridge::setLogger(log);}
	
private:

    struct Package
	{
		string name_;
		string version_;
		string env1_;
		string env2_;
		string symLink_;
		string exec_;
		string sanity_;
		bool complete()
		{
			return !name_.empty() && !version_.empty() && !env1_.empty() && !env2_.empty()
				&& !symLink_.empty() && !exec_.empty() && !sanity_.empty();
		}
	};
	
    struct Job
	{
		string command_;
		string name_;
		string new_version_;
		string restart_;
		string path_;
		string hard_;
		bool complete()
		{
			return !name_.empty() && !new_version_.empty() && !restart_.empty();
		}
	};
	int parseJob(ifstream& packageStream, Job& new_job);
	int parseUpdate(string updateFile, std::vector<Job>& job);
	int parseUpdateJob(ifstream& packageStream, Job& new_job);
	int parseRemoveJob(ifstream& packageStream, Job& new_job);
	int parseConfig(string packgeFile);
	int parsePackage(ifstream& packageStream);
	int parseAddPackage(ifstream& packageStream);
	int writeConfig();
	int doAddJob(Job& addJob);
	int doRemoveJob(Job& rmJob);
	int doUpdateJob(Job& upJob);
	int recoverExecutable(string recover_path, Package& package);
	string trim(string input);
	int sanityTimeSec_;
	UnixBridge ub_;	
    std::map<string,Package> packageMap_;
    std::map<string,Package> addMap_;
    string confFile_;
    string updatePath_;
    string updateFile_;
};

#endif
