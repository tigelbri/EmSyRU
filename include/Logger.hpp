#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <fstream>

#include "Timestamp.hpp"
using std::ofstream;
using std::ostream;

class Logger
{
public:

	/**
	 * @brief 	Ctor
	 */
    Logger();
    
    Logger(string filename);

    /**
     * @brief	Dtor
     */
    ~Logger();

    /**
     * @brief	Puts the given string into the current output sink.
     */
    void log(string event);

    /**
     * @brief	Sets output to given file.
     */
    void setOutputFile(string filename);

    /**
     * @brief	Sets output to stdout
     */
    void setOutputToStdout();


    /**
     * @brief	Output operator for output streams
     */
    friend ostream& operator<<(Logger& os, const string& str);

private:

    /// Timestamp for formatted output
    Timestamp 		m_timestamp;

    /// Output stream
    ostream* 		m_ofs;
};


#endif // LOGGER_HPP
