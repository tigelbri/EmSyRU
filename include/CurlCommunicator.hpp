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
* @brief Package class for holding and controling packages to update
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
//#define CURL_SIZEOF_LONG 4

/**
 * @brief   Pacakge class with all relevant package informations
 */
class CurlCommunicator
{

public:

	 CurlCommunicator();

    /**
    * @brief Ctor
    */
    CurlCommunicator(string user, string pw, int timeOutSec, int retries);

    /**
    * @brief Dtor
    */
    ~CurlCommunicator();
    int startUpload(string url, string saveFilename);
    int startDownload(string url, string saveFilename);
    static Logger log_;
	static void setLogger(Logger& log){log_ = log;}

private:

	string m_URL;
	string m_file;
	string user_;
	string pw_;
	int m_retries;
	int m_timeoutSec;
	int m_retryCount;
	
	int tryDownload(int tryCount);
	int tryUpload(int tryCount);
	
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
