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
        int findJobFile(string upPath, string& jobfile);
        int prepareWorkbench();
       
        string packageFile_ = "Packages.conf";
		string logFile_ = "job.log";
		string dlURL_;
		string upURL_;
		string env_;
		string jobFile_ = "Job.conf";
        Updater  up_;
        UnixBridge ub_;
		CurlCommunicator curli_;

		
};

#endif
