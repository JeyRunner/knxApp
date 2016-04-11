/* 
 * File:   Save.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Save CLASS
 * - stores config in sqlite db
 * - - stores values, containers
 * - exchange changes with server
 * --------------------------------------
 */


#ifndef KNX_SAVE_H
#define KNX_SAVE_H

// include
#include <iostream>
#include <string>
#include <stdlib.h>

#ifdef pl_andr
    #include "../android/include/sqlite3.h"
    #include <android/asset_manager_jni.h>
#else
#include <sqlite3.h>
#endif

#include "ui/DeviceValueView.h"
#include <list>
#include <functional>
#include <vector>
#include <Log.h>
#include "ui/ContainerView.h"
#include <SqlEntry.h>
class ContainerView;
class DeviceValueView;

using home::Log;
using namespace std;


/* Save class
 */
class Save
{
    public:
        static list<SqlEntry*>          sqlEntrys;
        static ContainerView *          containerRoot;
        static list<ContainerView *>    containers;
        static list<DeviceValueView *>  deviceValues;
        static list<SqlEntry*>          knxValues;

        static void init();                 // init local database
        static void close();                // close local database
        static bool sqlQuery(string sql);

        static void processConfigPacket();  // call after receive new config packed

        static SqlEntry* getSqlEntry(string id);


    private:
        static Log log;

        // database
        static sqlite3 *database;

        static void dbLoadElements(); // load elements form database
};


#endif /* KNX_SAVE_H */
