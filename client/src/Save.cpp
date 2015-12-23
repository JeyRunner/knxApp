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

#include "Save.h"
#include <iostream>

// static
sqlite3*          Save::database;
list<SqlEntry*>   Save::sqlEntrys;
Container*        Save::containerRoot;
list<Container*>  Save::containers;
vector<Save::OnNewSqlEntryDo*> Save::onNewSqlEntryDo;
Log               Save::log;


// -- INIT DATABASE ---------------------
void Save::init()
{
    log.setLogName("SAVE");


    // what to do when get new sqlElement
    onNewSqlEntryDo.push_back(new Save::OnNewSqlEntryDo("container",
                                                    [&](SqlEntry *entry) {
                                                        Container *container = new Container();
                                                        container->bindToSqlEntry(entry);
                                                    }));

    // how to generate id for entry from table
    SqlEntry::generateIdForTableList.push_back(new SqlEntry::GenerateIdForTable("container",
                                                                                [&] (SqlEntry *entry) {
                                                                                    return (entry->getField("id")->getValue()) + (entry->getField("localId")->getValue());
                                                                                }));
    SqlEntry::generateIdForTableList.push_back(new SqlEntry::GenerateIdForTable("containerValues",
                                                                                [&] (SqlEntry *entry) {
                                                                                    return (entry->getField("id")->getValue()) + (entry->getField("localId")->getValue());
                                                                                }));



    // open database
    #ifdef pl_andr
    char databasePath[] = "home.client.db";
    #else
    char databasePath[] = "/home/joshua/Schreibtisch/home.client.db";
    #endif
    sqlite3_stmt *stmt;

#ifdef pl_andr
    // retrieve the JNI environment.
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDLActivity
    jobject activity = (jobject)SDL_AndroidGetActivity();

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
    jclass sdlClass(env->GetObjectClass(activity));
    log.out("found java sdl class", sdlClass == NULL, "");


    jfieldID assman = env->GetStaticFieldID(sdlClass, "mAssetMgr", "Landroid/content/res/AssetManager;");
    if (assman == 0)
        log.err("could not find mAssetMgr");


    jobject assets = env->GetStaticObjectField(sdlClass, assman);
    if (assets == 0)
        log.err("could not get mAssetMgr");

    AAssetManager* manager = AAssetManager_fromJava((env), assets);
    log.ok("get AAssetManager");

    AAsset* dbFile = AAssetManager_open(manager, databasePath, AASSET_MODE_STREAMING);
    log.out("open db file", dbFile == NULL, "");

    off_t len = AAsset_getLength(dbFile);
#endif

    // open database
    bool ok = (sqlite3_open(databasePath, &database) == SQLITE_OK);
    log.out("open database", !ok,
        sqlite3_errmsg(database));



    // -- create tables if not exists
    string sql =    "CREATE TABLE IF NOT EXISTS containerValues(" \
                    "localId    INTEGER PRIMARY KEY AUTOINCREMENT," \
                    "id         INTEGER," \
                    "name       TEXT," \
                    "type       TEXT," \
                    "knxIn      TEXT," \
                    "knxOut     TEXT," \
                    "deviceId   INTEGER," \
                    "value      TEXT,"
                    "parentId   INTEGER," \
                    "changeNr   INTEGER," \
                    "uploaded   NUMERIC," \
                    "time       INTEGER" \
                    ");" \
                    "CREATE TABLE IF NOT EXISTS container( " \
                    "localId    INTEGER PRIMARY KEY AUTOINCREMENT," \
                    "id         INTEGER," \
                    "name       TEXT," \
                    "parentId   INTEGER," \
                    "changeNr   INTEGER," \
                    "uploaded   NUMERIC," \
                    "time       INTEGER" \
                    ");";


    // execute
    char *err;
    ok = (sqlite3_exec(database, sql.c_str(), NULL, NULL, &err) == SQLITE_OK);

    string errStr = ok ? "" : string(err);
    log.out("create tables (if not exists)", !ok, errStr);

    // load
    dbLoadElements();

    // find parents for container
    for (auto container : containers)
    {
        container->updateParent();
        //log.debug(container->sqlEntry->tableName);
        /*
        if (container->sqlEntry == NULL)
        {
            log.debug("container sqlEntry is NULL");
        }
        else
        {
            log.debug("container sqlEntry is ok  :" + container->sqlEntry->getField("name")->getValue());
        }*/
    }
}


// -- GET ACTION BY TABEL NAME ----------
void Save::onNewSqlEntry(string tableName, SqlEntry *entry)
{
    for (auto endo : onNewSqlEntryDo)
    {
        if (endo->tableName == tableName)
            endo->functionDo(entry);
    }
}


// -- LOAD ELEMENT FROM DB --------------
void Save::dbLoadElements()
{
    sqlite3_stmt *stmtListTables;

    // for each table
    string sqlListTables = "SELECT name FROM sqlite_master WHERE type='table'";
    bool ok = sqlite3_prepare_v2(database, sqlListTables.c_str(), sqlListTables.size(), &stmtListTables, 0) == SQLITE_OK;
    log.out("", !ok, sqlite3_errmsg(database));
    int cols = sqlite3_column_count(stmtListTables);

    while (sqlite3_step(stmtListTables) == SQLITE_ROW)
    {
        string col_name = "";
        string col_value = "";

        for (int i = 0; i < cols; i++)
        {
            if (sqlite3_column_name(stmtListTables, i))
                col_name = const_cast<const char *>(sqlite3_column_name(stmtListTables, i));

            if (sqlite3_column_text(stmtListTables, i))
                col_value = reinterpret_cast<const char *>(sqlite3_column_text(stmtListTables, i));

            if (col_name != "name")
                break;

            string tableName = col_value;
            if (tableName == "sqlite_sequence")
                break;


            // get entrys from table
            sqlite3_stmt *stmt;
            string sql = "SELECT * FROM " + tableName;

            bool ok = sqlite3_prepare_v2(database, sql.c_str(), sql.size(), &stmt, 0) == SQLITE_OK;
            log.out("load " + tableName, !ok,
                    sqlite3_errmsg(database));


            int cols = sqlite3_column_count(stmt);
            string col_name;
            string col_value;

            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                SqlEntry *sqlEntryNew = new SqlEntry(tableName, cols);
                sqlEntryNew->fromSql(stmt);

                // check if entry is already in list
                SqlEntry *sqlEntryOld = getSqlEntry(sqlEntryNew->getUniqueID());

                if (sqlEntryOld != nullptr)
                {
                    // exits already -> update
                    sqlEntryOld->fromSql(stmt);
                }
                else
                {
                    // perform action, add new
                    onNewSqlEntry(tableName, sqlEntryNew);
                    sqlEntrys.push_back(sqlEntryNew);
                }

            }
            sqlite3_finalize(stmt);
        }
    }
    sqlite3_finalize(stmtListTables);

    // set update all sqlElements -> find parent
    /*
    for (SqlElement *element : Save::sqlElements)
    {
        log.info("-- " + dynamic_cast<Container *>(element)->name);
        element->updateAll();
    }*/
}


// -- GET SQL ENTRY BY ID ---------------
SqlEntry *Save::getSqlEntry(string id)
{
    for (auto sqlEntry : sqlEntrys)
    {
        if (sqlEntry->getUniqueID() == id)
        {
            // found
            log.debug("getSqlEntry found " + id);
            return sqlEntry;
        }
    }
    return nullptr;
}


/*
// -- ADD ELEMENT TO DB -----------------
void Save::dbAddNewElement(SqlElement *element)
{
    sqlite3_stmt *stmt;
    string        sql, sqlFields, sqlValues;

    for (int i = 1 /*0 - reserved for key*-/; i < element->fieldsAmount; ++i)
    {
        string seperator = (i < element->fieldsAmount-1) ? "," : "";

        sqlFields+= "'" + element->getField_Name(i) + "'" + seperator;
        sqlValues+= "'" + element->getField(i) + "'" + seperator;
    }

    sql =  "INSERT INTO " + element->tableName + "(" + sqlFields + ") VALUES(" + sqlValues + ");";

    bool ok = sqlite3_prepare_v2(database, sql.c_str(), sql.size(), &stmt, 0) == SQLITE_OK;
    ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    // set local id
    element->localId = sqlite3_last_insert_rowid(database);

    log.out("insert element |" + sql + "| localId=" + to_string(element->localId), !ok,
        sqlite3_errmsg(database));

    // search for parent, ...
    element->updateAll();

    // add to own list
    sqlElements.push_back(element);
}

// -- UPDATE ELEMENT IN DB --------------
void Save::dbUpdateElement(SqlElement *element)
{
    sqlite3_stmt *stmt;
    string        sql, sqlData, sqlWhere;

    // element need upload
    element->uploaded = false;

    for (int i = 1 /*0 - reserved for key*-/; i < element->fieldsAmount; ++i)
    {
        string seperator = (i < element->fieldsAmount-1) ? "," : "";

        sqlData+= "'" + element->getField_Name(i) + "'=";
        sqlData+= "'" + element->getField(i) + "'" + seperator;
    }


    // take available id
    if (element->id > -1)
        sqlWhere = "id=" + to_string(element->id);
    else if (element->localId > -1)
        sqlWhere = "localid=" + to_string(element->localId);
    else
        log.out("get sql elements available id " + to_string(element->localId), true, "id and localId not set");


    sql =  "UPDATE " + element->tableName + " SET " + sqlData + " WHERE "+ sqlWhere +";";

    bool ok = sqlite3_prepare_v2(database, sql.c_str(), sql.size(), &stmt, 0) == SQLITE_OK;
    ok = sqlite3_step(stmt) == SQLITE_DONE;
    log.out("update element |" + sql + "|", !ok,
        sqlite3_errmsg(database));
}

// -- REMOVE ELEMENT FROM DB ------------
void Save::dbRemoveElement(SqlElement *element)
{
}
*/


// -- PROCESS PACKET --------------------
void Save::processConfigPacket()
{

}



// -- CLOSE DATABASE --------------------
void Save::close()
{
    // close database
    bool ok = sqlite3_close(database) == SQLITE_OK;
    log.out("close database ", !ok, "");
}