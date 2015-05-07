/*@author pklein,mbremer *
 * Diese Klasse fügt Programmabschnitten einen Zeitstempel hinzu. 
 *Mittels gettimeofday() des headers <sys/time.h> analysieren wir die Laufzeiten dieser Programmabschnitte
 * in millisekunden, Sekunden, Minuten , stunden und geben dies auf der Konsole durch einen
 *eigens entworfenen <<-Operator aus. 
 */

#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP

#include <string>
#include <sys/time.h>
#include <sstream>

using std::string;
using std::ostream;

/**
* @brief A helper class for automated time stamping. Timing is
*
started as soon as an object of this class is created.
*
To time some parts of a program, just create a new object
*
and use the provided output operator to display the elapsed
*
time.
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
