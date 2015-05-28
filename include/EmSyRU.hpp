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
* @file EmSyRU.hpp
* @brief Embedded System Remote Updater
* @author Tristan Igelbrink
* @date 20.03.2015
*/

#ifndef EmSyRU_HPP
#define EmSyRU_HPP

#include "Updater.hpp"
#include "UnixBridge.hpp"
#include "CurlCommunicator.hpp"
#include "Logger.hpp"
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>


using std::string;
using std::cout;
using std::endl;

/**
 * @brief  Class for Emdedded System Remote Updater
 */
class EmSyRU
{

public:
    /**
    * @brief Ctor
    */
    EmSyRU(string user, string dlURL, string pw, string env  = "");
    

    /**
    * @brief Dtor
    */
    ~EmSyRU();

	int update();
	
	static Logger log_;
	static void setLogger(Logger& log){log_ = log; UnixBridge::setLogger(log);}
	
private:

        void uploadLog();
        int findJobConfFile();
        int prepareWorkbench();
       
        string packageFile_ = "Packages.conf";
		string logFile_ = "job.log";
		string dlURL_;
		string upURL_;
		string env_;
		string jobFile_;
		string jobConfFile_ = "Job.conf";
        Updater  up_;
        UnixBridge ub_;
		CurlCommunicator curli_;

		
};

#endif
