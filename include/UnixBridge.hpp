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

#ifndef UNIXBRIDGE_HPP
#define UNIXBRIDGE_HPP

/**
* @file UnixBridge.hpp
* @brief Class for interaction with unix systems 
* @author Tristan Igelbrink
* @date 18.03.2015
*/

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
    
	/**
	* Creates a symbolic link to given file
	* @param string with name of symbolic link
	* @param string with file to link to
	* @return int with succsess 1 or fail 0
	*/
    int createSymbolicLink(string name, string content);

	/**
	* Read the content of a symbolic link
	* @param string link to read
	* @return string with content if fail ""
	*/
    string getSymLink(string link);
    
    /**
	* C++ wrapper for unix mv
	* @param string with file to move
	* @param string target
	* @return int with succsess 1 or fail 0
	*/
    int mvFile(string in, string out);

	/**
	* C++ wrapper for unix rm -r
	* @param string with directory to delete
	* @return int with succsess 1 or fail 0
	*/
    int deleteDir(string dir);
    
    /**
	* C++ wrapper for unix mv -r
	* @param string with directory to move
	* @param string with target
	* @return int with succsess 1 or fail 0
	*/
	int	mvDir(string move_from, string move_to);
	
	/**
	* Collects all filenames of a given directory and can skip a config file
	* @param string with target directory
	* @param string with file name to skip
	* @param vector with filenames
	* @return int with succsess 1 or fail 0
	*/
	int getFiles(string dir, string upFile, std::vector<string>& files);
	
	/**
	* Collects all filenames of a given directory and can skip a config file
	* @param string with directory name
	* @param mode_t set unix rights for new directory
	* @return int with succsess 1 or fail 0
	*/
	int createDir(string dir, mode_t mode);

	
	static Logger log_; /**< Logger for tracking and generating report to upload */
	
	/**
	* Setter for logger
	* @param Logger
	*/
	static void setLogger(Logger& log){log_ = log;}

};


#endif // UNIXBRIDGE_HPP
