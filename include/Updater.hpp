/**
* @file Package.hpp
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
#include <dirent.h>
#include <errno.h>
#include <map>
#include <fstream>
#include <regex>
#include <dirent.h>
#include "Timestamp.hpp"
#include "Logger.hpp"
#include <fts.h>
#include <sys/stat.h>

/*extern void* xrealloc (void *ptr, size_t size);
extern char* readlink_malloc (const char *filename);
extern int getProcIdByName(std::string procName);
extern void signalHandler(int signum);
extern void registerHandler();
extern bool SIGNALRECEIVED;*/


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
	int getUpdateFiles(string dir, string upFile, std::vector<string>& files);
	int createDir(string dir, mode_t mode);
	
	static Logger log_;
	static void setLogger(Logger& log){log_ = log;}
	
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
	int writeUpdateFile(string in, string out);
	int doAddJob(Job& addJob);
	int doRemoveJob(Job& rmJob);
	int doUpdateJob(Job& upJob);
	int recoverExecutable(string recover_path, Package& package);
	string getCurrentAppfolder(string link);
	string trim(string input);
	int createSymbolicLink(string name, string content);
	int sanityTimeSec_;
	int deleteDir(string dir);
	int	moveFiles(string move_from, string move_to);	
    std::map<string,Package> packageMap_;
    std::map<string,Package> addMap_;
    string confFile_;
    string updatePath_;
    string updateFile_;
};

#endif
