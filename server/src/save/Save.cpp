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

#include "Save.h"

// static
sqlite3*  Save::database;
home::Log Save::log;


// -- INIT DATABASE ---------------------
void Save::init()
{
    log.setLogName("SAVE", "Save");

    char databasePath[] = "/home/joshua/Schreibtisch/home.server2.db";
    sqlite3_stmt *stmt;

    // open database
    bool ok = (sqlite3_open(databasePath, &database) == SQLITE_OK);
    log.out("open database", !ok,
        sqlite3_errmsg(database));


    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :: create tables if not exists :::::::::::::::::::::::::::::::::::::::
    string sql =    "CREATE TABLE IF NOT EXISTS deviceValues(" \
                    "localId    INTEGER PRIMARY KEY AUTOINCREMENT," \
                    "id         INTEGER," \
                    "name       TEXT," \
                    "type       TEXT," \
                    "knxIn      TEXT," \
                    "knxOut     TEXT," \
                    "deviceId   INTEGER," \
                    "value      TEXT," \
                    "changeNr   INTEGER," \
                    "uploaded   NUMERIC," \
                    "time       INTEGER" \
                    ");" \
                    "CREATE TABLE IF NOT EXISTS container( " \
                    "localId    INTEGER PRIMARY KEY AUTOINCREMENT," \
                    "id         INTEGER," \
                    "name       TEXT," \
                    "changeNr   INTEGER," \
                    "uploaded   NUMERIC," \
                    "time       INTEGER" \
                    ");";


    // execute
    char *err;
    ok = (sqlite3_exec(database, sql.c_str(), NULL, NULL, &err) == SQLITE_OK);

    string errStr = ok ? "" : string(err);
    log.out("create tables (if not exists)", !ok, errStr);
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



    // delete content
    sql = "DELETE FROM deviceValues;";

    // compile
    sqlite3_prepare_v2(database, sql.c_str(), sql.size(), &stmt, NULL);
    log.out("delete data", sqlite3_step(stmt) != SQLITE_DONE, sqlite3_errmsg(database));



    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :: fill in data ::::::::::::::::::::::::::::::::::::::::::::::::::::::
    sql = "INSERT INTO deviceValues(name, type) VALUES (@name, @type);";

    // compile
    sqlite3_prepare_v2(database, sql.c_str(), sql.size(), &stmt, NULL);
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // some data
    sql = "";
    for (int i = 0; i < 10; ++i)
    {
        // bind values
        // sqlite3_bind_int(stmt, 1, i);
        sqlite3_bind_text(stmt, 1, string(string("Value ") + to_string(i)).c_str(), -1, SQLITE_TRANSIENT /* not free after use */);
        sqlite3_bind_text(stmt, 2, "switch", -1, SQLITE_TRANSIENT /* not free after use */);

        // execute insert
        sqlite3_step(stmt);

        // change something with rowid
        int id = sqlite3_last_insert_rowid(database);
        sql = "UPDATE deviceValues SET time=" + to_string(id) + "00001 WHERE id=" + to_string(id) + ";";
        sqlite3_exec(database, sql.c_str(), NULL, NULL, NULL);
        //cout << id << endl;

        // reset
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_exec(database, "END TRANSACTION", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    log.out("insert data fished", false);
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :: show ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    sql = "SELECT * FROM deviceValues";

    ok = sqlite3_prepare_v2(database, sql.c_str(), sql.size(), &stmt, 0) == SQLITE_OK;
    log.out("show table", !ok,
        sqlite3_errmsg(database));


    int cols = sqlite3_column_count(stmt);
    string col_name;
    string col_value;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string row;
        for (int i=0;i<cols;i++)
        {
            if(sqlite3_column_name(stmt,i))
                col_name = const_cast<const char*>(sqlite3_column_name(stmt,i));
            else col_name = "LEER";

            if (sqlite3_column_text(stmt,i))
                col_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt,i));
            else col_value = "LEER";

            row.append(col_name + ": ").append(col_value).append("(" + to_string(sqlite3_column_type(stmt,i)) + ")   ");
        }
        //cout << row << endl;
    }
    sqlite3_finalize(stmt);

}



// -- PROCESS PACKET --------------------
void Save::processConfigPacket()
{

}


// -- CLOSE DATABASE --------------------
void Save::close()
{
    // close database
    bool ok = sqlite3_close(database) == SQLITE_OK;
    log.out("close database ", !ok, sqlite3_errmsg(database));
}