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

#include "Timestamp.hpp"

#include <iomanip>

Timestamp::Timestamp()
{
    resetTimer();
}

unsigned long Timestamp::getCurrentTimeInMs() const
{
    timeval tv;
    gettimeofday(&tv, 0);
    
    unsigned long mili  = tv.tv_sec*1000 + tv.tv_usec/1000;

    return mili;
}

unsigned long Timestamp::getElapsedTimeInMs() const
{
    return getCurrentTimeInMs() - m_startTime;
}

double Timestamp::getCurrentTimeinS() const
{
    return (double) getCurrentTimeInMs()/1000;
    
}

double Timestamp::getElapsedTimeInS() const
{
    return (double) (getCurrentTimeInMs() - m_startTime)/1000;
}

void Timestamp::resetTimer()
{
    m_startTime = getCurrentTimeInMs();
}

string Timestamp::getElapsedTime() const
{
    std::stringstream stringTime;
    
    unsigned long mili        = getElapsedTimeInMs();
    unsigned long hours       = mili/1000/60/60;
    unsigned long minutes     = mili/1000/60 - (hours * 60);
    unsigned long seconds     = mili/1000 - (minutes * 60) - (hours * 60 * 60);
    unsigned long miliseconds = mili - (minutes * 60 * 1000) - (hours * 60 * 60 * 1000) - seconds * 1000;
    std::stringstream ms;
    if(miliseconds < 10)
    {
        ms << "0";
    }
    if(miliseconds < 100)
    {
        ms << "0";
    }
    ms << miliseconds;
    
    stringTime  << "[" << std::setfill ('0') << std::setw (2) << hours << ":"
    		           << std::setfill ('0') << std::setw (2) << minutes << ":"
    		           << std::setfill ('0') << std::setw (2) << seconds << " - "
    		           << std::setfill ('0') << std::setw (3) << ms.str() << "]";
    return stringTime.str();
}


ostream& operator<<(ostream& os, const Timestamp& ts)
{
    os << ts.getElapsedTime();
    return os;
}
