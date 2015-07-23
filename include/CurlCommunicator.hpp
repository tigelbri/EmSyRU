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
* @file CurlCommunicator.hpp
* @brief class for communication
* @author Tristan Igelbrink
* @date 20.03.2015
*/

#ifndef CurlCommunicator_HPP
#define CurlCommunicator_HPP


#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <cstdlib>
#include <cstring>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "Logger.hpp"
#include <unistd.h>	

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace curlpp::options;

/**
 * @brief   Class for communication 
 * This class provides the communication for downloading job files or uploading log files.
 * To communicate with the server the curl libary is used
 */
class CurlCommunicator
{

public:

	/**
    * @brief Ctor
    */
	 CurlCommunicator();

    /**
    * @brief Ctor
    * 
    *@param string with user name
    *@param string with password
    *@param int with time for connection timeout in seconds
    *@param int with amount of retries to make before say connection falied
    */
    CurlCommunicator(string user, string pw, int timeOutSec, int retries);

    /**
    * @brief Dtor
    */
    ~CurlCommunicator();
    
    /**
	* Upload a given file to the given url
	* @param string with url
	* @param string with file to upload
	* @return int with succsess 1 or fail 0
	*/
    int startUpload(string url, string saveFilename);
    
    /**
	* Downloads a requested file from the given url
	* @param string with url
	* @param string with file to download to
	* @return int with succsess 1 or fail 0
	*/
    int startDownload(string url, string saveFilename);
    
    
    static Logger log_; /**< Logger for tracking and generating report to upload */
    
    /**
	* Setter for logger
	* @param Logger
	*/
	static void setLogger(Logger& log){log_ = log;}

private:

	string m_URL;        /**< URL to work with */
	string m_file;       /**< file to upload */
	string user_;        /**< username  for server */
	string pw_;          /**< password for server */
	int m_retries;       /**< number of retries for connection */
	int m_timeoutSec;    /**< time in seconds to wait befor timing out */
	int m_retryCount;    /**< actual number of retry */
	
	/**
	* One single donwload try
	* If it fails or is interrupted the Download is restarted at the old byteoffset 
	* @param int with actual retry
	* @return int with succsess 1 or fail 0
	*/
	int tryDownload(int tryCount);
	
	/**
	* One single upload try
	* If it fails or is interrupted the Upload is restarted at the old byteoffset 
	* @param int with actual retry
	* @return int with succsess 1 or fail 0
	*/
	int tryUpload(int tryCount);
	
	/**
	 * @brief  Internal helper class for writing downloaded files  
	 */
	struct CurlWriter
	{
	public:
	  CurlWriter(){};
	
	  CurlWriter(std::ostream* stream, int retries, int* count)
		: mStream(stream), retries_(retries), count_(count)
	  {}

	  // Helper Class for reading result from remote host
	  size_t write(curlpp::Easy *handle, char* ptr, size_t size, size_t nmemb)
	  {
		if(*count_ != retries_)
			*count_ = retries_;
		// Calculate the real size of the incoming buffer
		size_t realsize = size * nmemb;
		mStream->write(ptr, realsize);
		// return the real size of the buffer...
		return realsize;
	  };

	  std::ostream * mStream;
	  int retries_;
	  int* count_;
	};


	/**
	 * @brief  Internal helper class for checking the communication progress 
	 */
	class CurlProgress
	{
	public:
		 CurlProgress()
		: m_complete(0)
		, m_total(0)
		{}
		
		int progressCallback(double dltotal, double dlnow, double ultotal, double ulnow)
		{
			if(dlnow != 0 && dltotal != 0)
			{
				m_complete = dlnow;
				m_total = dltotal;
			}
			return 0;
		}
		double m_complete;
		double m_total;
	};
	
	CurlWriter m_writer;
	CurlProgress m_progress;
	

};

#endif
