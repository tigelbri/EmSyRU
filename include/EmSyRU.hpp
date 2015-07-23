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
* @file EmSyRU.hpp
* @brief class with EmSyRU application 
* @author Tristan Igelbrink
* @date 24.03.2015
*/

/**
 * @brief  Class for Emdedded System Remote Updater
 */
class EmSyRU
{

public:
    /**
    * @brief Ctor
    * 
    *@param string with user name
    *@param string with url
    *@param string with password
    *@param optional string with enviroment to extract files
    */
    EmSyRU(string user, string dlURL, string pw, string env  = "");
    

    /**
    * @brief Dtor
    */
    ~EmSyRU();

	/**
	* Starts the whole update process
	* This Method uses the given url user and pw to download a jobfile
	* extracting and scanning it and updates its packages
	* @return int with succsess 1 or fail 0
	*/
	int update();
	
	static Logger log_; /**< Logger for tracking and generating report to upload */
	
	/**
	* Setter for logger
	* @param Logger
	*/
	static void setLogger(Logger& log){log_ = log; UnixBridge::setLogger(log);}
	
private:

		/**
		* Function for uploading the generated logfile to given url
		*/
        void uploadLog();
		
		/**
		* Searches for job config file in downloaded job file
		* @return int with succsess 1 or fail 0
		*/
        int findJobConfFile();
        
        /**
		* Checks if the workbench is clean and works it off if not
		* @return int with succsess 1 or fail 0
		*/
        int prepareWorkbench();
       
        string packageFile_ = "Packages.conf";  /**< Path to Package DB*/
		string logFile_ = "job.log";            /**< Logfile for tracking*/
		string dlURL_;                          /**< URL to download */
		string upURL_;                          /**< URL to upload*/
		string env_;                            /**< place extract job file*/
		string jobFile_;                        /**< Downloaded job file*/
		string jobConfFile_ = "Job.conf";       /**< job config file determined*/
        Updater  up_;                           /**< Handles the update process*/
        UnixBridge ub_;                         /**< Handles connection to unix*/
		CurlCommunicator curli_;                /**< Handles server communication*/

		
};

#endif
