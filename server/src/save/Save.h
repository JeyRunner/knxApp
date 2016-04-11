/* 
 * File:   Save.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Save CLASS - serverSite
 * - stores config in sqlite db
 * - exchange changes with clients
 * --------------------------------------
 */


#ifndef KNX_SAVE_H
#define KNX_SAVE_H

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sqlite3.h>
#include "Log.h"

using namespace std;


/* Save class
 */
class Save
{
    public:
        static void init();                 // init local database
        static void close();                // close local database
        static void processConfigPacket();  // call after receive new config packed

    private:
        // database
        static sqlite3 *database;
        static home::Log log;
};

#endif /* KNX_SAVE_H */
