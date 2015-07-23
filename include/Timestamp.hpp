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

#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP

/**
* @file Timestamp.hpp
* @brief Logger class for report generation 
* @author Tristan Igelbrink
* @date 18.03.2015
*/

#include <string>
#include <sys/time.h>
#include <sstream>

using std::string;
using std::ostream;

/**
* @brief A helper class for automated time stamping. 
* Timing is started as soon as an object of this class is created.
*
* To time some parts of a program, just create a new object
*
* and use the provided output operator to display the elapsed time.
*/
class Timestamp
{
	public:
	/**
	* @brief
	Constructor.
	*/
	Timestamp();

	/**
	* @brief
	Returns the current system time in milliseconds
	*/
	unsigned long getCurrentTimeInMs() const;

	/**
	* @brief
	Returns the milliseconds since object creation
	*/
	unsigned long getElapsedTimeInMs() const;

	/**
	* @brief Returns the current system time in seconds
	*/
	double getCurrentTimeinS() const;

	/**
	* @briefReturns the time since instantiation in seconds

	*/
	double getElapsedTimeInS() const;

	/**
	* @brief Resets the internal timer
	*/
	void resetTimer();

	/**
	* @brief Returns a string representation of the current timer value
	*
	*/
	string getElapsedTime() const;

	friend ostream& operator<<(ostream& os, const Timestamp& ts);

private:

	/// The system time at object instantiation
	unsigned long m_startTime;

	timeval getTimeval(); 
};


#endif // TIMESTAMP_HPP
