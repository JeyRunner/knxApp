/* 
 * File:   Log.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Log CLASS
 * loggable object
 * --------------------------------------
 */


#ifndef _KNX_LOG_H
#define _KNX_LOG_H

// include
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;
namespace home
{

/* Log class
 */
class Log
{
    public:
        static void setLogLevel(char log_level_mask);
        static void setUseLongLogNames(bool use);

        void setLogName(string nameShort); // set log name example: class name
        void setLogName(string nameShort, string nameLong); // set log name example: class name

        void err(string text);  // error message
        void warn(string text); // warning message
        void ok(string text);   // ok message
        void debug(string text);// debug message
        void info(string text); // information message
        void trace(string text);// trace info message

        void out(string text, bool error, string errorText); // ok/err - also print error if ok=false
        void out(string text, bool error);                   // ok/err - also print error if ok=false

    private:
        string logNameShort, logNameLong;
        static char logLevelMask;
        static bool useLongLogNames;
        static int  maxNameLongLen;

        void print(string text);
};


/* LOG LEVEL
 */
const char  LOG_LEVEL_NOTHING       = 0b00000000,
            LOG_LEVEL_ERROR         = 0b00000001,
            LOG_LEVEL_WARNING       = 0b00000010,
            LOG_LEVEL_OK            = 0b00000100,
            LOG_LEVEL_DEBUG         = 0b00001000,
            LOG_LEVEL_INFORMATION   = 0b00010000,
            LOG_LEVEL_TRACE         = 0b00100000,
            LOG_LEVEL_ALL           = 0b11111111;


// to_string for android
template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

template <typename T>
std::string str(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

};     /* END NAMESPACE */
#endif /* _KNX_LOG_H */
