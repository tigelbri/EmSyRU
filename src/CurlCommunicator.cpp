#include "CurlCommunicator.hpp"

Logger CurlCommunicator::log_ = Logger();

CurlCommunicator::CurlCommunicator()
{
	
} 

/**
* @brief Ctor
*/
CurlCommunicator::CurlCommunicator(int timeOutSec, int retries) 
	: m_retries(retries), m_timeoutSec(timeOutSec), m_retryCount(retries)
{

}

/**
* @brief Dtor
*/
CurlCommunicator::~CurlCommunicator()
{
	
}

int CurlCommunicator::startUpload(string url, string saveFilename)
{
	m_URL = url;
	m_file = saveFilename;
	m_progress = CurlProgress();
	return tryUpload(m_retries);
}

int CurlCommunicator::startDownload(string url, string saveFilename)
{
	m_URL = url;
	m_progress = CurlProgress();
	ofstream outfile(saveFilename.c_str());
	m_writer = CurlWriter(&outfile, m_retries, &m_retryCount);
	return tryDownload(m_retries);
}

int CurlCommunicator::tryDownload(int tryCount)
{
	try
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;
		curlpp::types::WriteFunctionFunctor functor(utilspp::BindFirst(utilspp::make_functor(&m_writer, &CurlWriter::write), &request));
		
		using namespace curlpp::Options;
		request.setOpt(new WriteFunction(functor));
		request.setOpt(new Url(m_URL));
		request.setOpt(new UserPwd("ftp622_4:Vp2V99p2q4"));
		request.setOpt(new Verbose(false));
		request.setOpt(new NoProgress(false));
		if(m_progress.m_complete > 0)
		{
			request.setOpt(new ResumeFrom(m_progress.m_complete));
		}
		request.setOpt(new LowSpeedLimit(10));
		request.setOpt(new LowSpeedTime(5));
		request.setOpt(new ConnectTimeout(m_timeoutSec));
		
		request.setOpt(ProgressFunction(curlpp::types::ProgressFunctionFunctor(&m_progress, 
			&CurlProgress::progressCallback)));
		request.perform();
	
	}
	catch(curlpp::RuntimeError & e)
	{
		
		if(!string(e.what()).compare("RETR response: 550"))
		{
			log_ << "STATUS: No job file found! " << endl;
			return 0;
		}
		
		if(m_progress.m_complete == 0 && m_progress.m_total == 0)
		{
			// Couldnt connect to server 
			log_ << "ERROR: Failed to connect to server. Retries left: " << m_retryCount << endl;
			usleep(3000000);
			if(m_retryCount == 0) return 2;
			tryDownload(m_retryCount--);
		}	
		else
		{
			// Connection interrupted
			log_ << "WARNING: Connection interrrupted, Retries left: " << m_retryCount << endl;
			usleep(3000000);
			if(m_retryCount == 0) return 2;
				tryDownload(m_retryCount--);
		}
		log_ << "FATAL ERROR: " << e.what() << std::endl;
		return 2;
	}
	catch(curlpp::LogicError & e)
	{
		log_ << "FATAL LOGIC ERROR: " << e.what() << std::endl;
		return 2;
	}
	return 1;
}


int CurlCommunicator::tryUpload(int tryCount)
{
	try
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;
		std::ifstream logfile(m_file);
		if(!logfile)
			return log_ << "ERROR: Log file doesnt exist " << endl, 0;
		std::stringstream buffer;
		buffer << logfile.rdbuf();
		int size = buffer.str().size();
		logfile.close();
		
        using namespace curlpp::Options;
		request.setOpt(new ReadStream(&buffer));
		request.setOpt(new InfileSize(size));
		request.setOpt(new Upload(true));
		request.setOpt(new Url(m_URL));
		//request.setOpt(new UserPwd("pi:huff3llpuff"));
		request.setOpt(new UserPwd("ftp622_4:Vp2V99p2q4"));
		request.setOpt(new Verbose(false));
		request.setOpt(new NoProgress(false));
		if(m_progress.m_complete > 0)
		{
			request.setOpt(new ResumeFrom(m_progress.m_complete));
		}
		request.setOpt(new LowSpeedLimit(10));
		request.setOpt(new LowSpeedTime(5));
		request.setOpt(new ConnectTimeout(m_timeoutSec));
		
		request.setOpt(ProgressFunction(curlpp::types::ProgressFunctionFunctor(&m_progress, 
			&CurlProgress::progressCallback)));
		request.perform();
	
	}
	catch(curlpp::RuntimeError & e)
	{
		if(m_progress.m_complete == 0 && m_progress.m_total == 0)
		{
			// Couldnt connect to server 
			log_ << "ERROR: Failed to connect to server. Retries left: " << m_retryCount << endl;
			usleep(3000000);
			if(m_retryCount == 0) return 2;
			tryDownload(m_retryCount--);
		}	
		else
		{
			// Connection interrupted
			log_ << "WARNING: Connection interrrupted, Retries left: " << m_retryCount << endl;
			usleep(3000000);
			if(m_retryCount == 0) return 2;
			tryDownload(m_retryCount--);
		}
		log_ << "FATAL ERROR: " << e.what() << std::endl;
		return 2;
	}
	catch(curlpp::LogicError & e)
	{
		log_ << "FATAL LOGIC ERROR: " << e.what() << std::endl;
		return 2;
	}
	return 1;
}
	
