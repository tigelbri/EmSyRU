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
