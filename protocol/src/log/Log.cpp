/* 
 * File:   Log.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Log CLASS
 * @...
 * --------------------------------------
 */

#include "Log.h"
#ifndef pl_pi
#endif

using namespace home;

// static
char Log::logLevelMask    = LOG_LEVEL_ALL;
bool Log::useLongLogNames = false;
int  Log::maxNameLongLen  = 0;


// -- SET LOG NAME ----------------------
void Log::setLogName(string nameShort)
{
    this->logNameShort = nameShort;
    this->logNameLong  = "";
}
void Log::setLogName(string nameShort, string nameLong)
{
    setLogName(nameShort);
    this->logNameLong = nameLong;

    if (logNameLong.length() > maxNameLongLen)
        maxNameLongLen = logNameLong.length();
}

void Log::setLogLevel(char log_levelMask)
{
    logLevelMask = log_levelMask;
}

void Log::setUseLongLogNames(bool use)
{
    useLongLogNames = use;
}


// -- OUT -------------------------------
void Log::err(string text)
{
    if ((logLevelMask & LOG_LEVEL_ERROR) > 0)
        print("[ err  ] " + text);
}

void Log::warn(string text)
{
    if ((logLevelMask & LOG_LEVEL_WARNING) > 0)
        print("[ warn ] " + text);
}

void Log::ok(string text)
{
    if ((logLevelMask & LOG_LEVEL_OK) > 0)
        print("[  ok  ] " + text);
}

void Log::debug(string text)
{
    if ((logLevelMask & LOG_LEVEL_DEBUG) > 0)
        print("[debug ] " + text);
}

void Log::info(string text)
{
    if ((logLevelMask & LOG_LEVEL_INFORMATION) > 0)
        print("[ info ] " + text);
}

void Log::trace(string text)
{
    if ((logLevelMask & LOG_LEVEL_TRACE) > 0)
        print("[ trac ] " + text);
}

void Log::out(string text, bool error, string errorText)
{
     if (error && ((logLevelMask & LOG_LEVEL_ERROR) > 0))
         print("[ err  ] " + text + ": '" + errorText + "'");
     else
         this->ok(text);
}

void Log::out(string text, bool error)
{
    out(text, error, "");
}



void Log::print(string text)
{
    if (!useLongLogNames)
        cout << "[ " << logNameShort.substr(0, 4) << " ] " << text << endl;
    else
    {
        if (logNameLong != "")
            cout << "[ " << logNameLong.substr(0, maxNameLongLen) << " ] " << text << endl;
        else
            cout << "[ " << logNameShort.substr(0, maxNameLongLen) << " ] " << text << endl;

    }


    // android
#ifdef pl_andr
    string out =  "[ " + logName + " ] " + text;
    SDL_Log(out.c_str());
#endif
}
