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
