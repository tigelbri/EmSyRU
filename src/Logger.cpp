#include "Logger.hpp"

Logger::Logger()
{
	// Set initial output to std::cout
	m_ofs = &std::cout;
}

Logger::Logger(string filename)
{
    m_ofs = new ofstream(filename.c_str(), ofstream::out);
}

Logger::~Logger()
{

}

void Logger::log(string event)
{
   *m_ofs << m_timestamp << " " << event;
}

void Logger::setOutputFile(string filename)
{
    m_ofs = new ofstream(filename.c_str(), ofstream::out);
}

void Logger::setOutputToStdout()
{
    m_ofs = &std::cout;
}

ostream& operator<<(Logger& lg, const string& str)
{
    lg.log(str);
    return *(lg.m_ofs);
}
