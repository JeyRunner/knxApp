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

#include <list>
#include <functional>
#include <vector>
#include <Log.h>
#include "ui/Container.h"
#include <SqlEntry.h>
class Container;

using namespace std;


/* Save class
 */
class Save
{
    public:
        static list<SqlEntry*> sqlEntrys;
        static Container*      containerRoot;
        static list<Container*>containers;

        static void init();                 // init local database
        static void close();                // close local database
        static void processConfigPacket();  // call after receive new config packed

        static SqlEntry* getSqlEntry(string id);

    /*
        static void dbAddNewElement(SqlElement *element); // add NEW element to database
        static void dbUpdateElement(SqlElement *element); // update element in database
        static void dbRemoveElement(SqlElement *element); // remove element form database
    */

        struct OnNewSqlEntryDo
        {
            OnNewSqlEntryDo(string table, function<void (SqlEntry *entry)> func)
            { tableName = table; functionDo = func; }
            string tableName;
            function<void (SqlEntry *entry)> functionDo;
        };
        static vector<OnNewSqlEntryDo*> onNewSqlEntryDo;
        static void onNewSqlEntry(string tableName, SqlEntry *entry);


    private:
        static Log log;

        // database
        static sqlite3 *database;

        static void dbLoadElements(); // load elements form database
};

// static
//sqlite3*          Save::database;
//list<SqlElement*> Save::sqlElements;
//Container*        Save::containerRoot;


#endif /* KNX_SAVE_H */
