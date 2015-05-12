#ifndef UNIXBRIDGE_HPP
#define UNIXBRIDGE_HPP

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h> 
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "Logger.hpp"
#include <signal.h>
#include <fstream>
#include <regex>

using namespace std;

class UnixBridge
{
public:

	/**
	 * @brief 	Ctor
	 */
    UnixBridge();
    
    /**
     * @brief	Dtor
     */
    ~UnixBridge();
    
    int createSymbolicLink(string name, string content);

    string getSymLink(string link);
    
    int mvFile(string in, string out);

    int deleteDir(string dir);
    
	int	mvDir(string move_from, string move_to);
	
	int getFiles(string dir, string upFile, std::vector<string>& files);
	
	int createDir(string dir, mode_t mode);

	static Logger log_;
	static void setLogger(Logger& log){log_ = log;}

};


#endif // UNIXBRIDGE_HPP
