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
sqlite3*                Save::database;
list<SqlEntry*>         Save::sqlEntrys;
ContainerView *         Save::containerRoot;
list<ContainerView *>   Save::containers;
list<DeviceValueView *> Save::deviceValues;
list<SqlEntry*>         Save::knxValues;
Log                     Save::log;


// -- INIT DATABASE ---------------------
void Save::init()
{
    log.setLogName("SAVE");


    // -- open database
#ifdef pl_andr
    char databasePath[] = "/data/data/com.pancake.knxApp/files/home.client.db"; //"/data/data/com.pancake.knxApp/databases/home.client.db";
    #else
    char databasePath[] = "/home/joshua/Schreibtisch/home.client.db";
#endif
    sqlite3_stmt *stmt;

#ifdef pl_adndr
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




    // set entry sqlQuery function
    SqlEntry::sqlQuery = ([&](string sql){ return sqlQuery(sql); });
    SqlEntry::database = database;

    // what to do when get new sqlElement
    SqlEntry::onNewEntryOfTableRules = {new SqlEntry::OnNewEntryOfTable("container",
                                                                  [&](SqlEntry *entry)
                                                                  {
                                                                      ContainerView *container = new ContainerView();
                                                                      container->bindToSqlEntry(entry);
                                                                  }),
                                        new SqlEntry::OnNewEntryOfTable("deviceValues",
                                                                        [&](SqlEntry *entry)
                                                                        {
                                                                            DeviceValueView *valueView;

                                                                            string type = entry->getField("type")->getValue();
                                                                            if (type == "switch")
                                                                                valueView = new SwitchDeviceValue();
                                                                            else
                                                                                valueView = new DeviceValueView();

                                                                            valueView->bindToSqlEntry(entry);
                                                                        }),
                                        new SqlEntry::OnNewEntryOfTable("knxValues",
                                                                        [&](SqlEntry *entry)
                                                                        {
                                                                            log.debug("new knxValue");
                                                                            knxValues.push_back(entry);
                                                                            string valueId = entry->getField("valueId")->getValue();

                                                                            // put to fitting deviceValue
                                                                            for (DeviceValueView *valueView : deviceValues)
                                                                            {
                                                                                //log.debug("knxValue("+entry->getField("valueId")->getValue()+") find deviceValue / candidat: " + valueView->fieldId->getValue());
                                                                                if (valueView->sqlEntry->getField("id")->getValue() == valueId)
                                                                                {
                                                                                    log.ok("knxValue("+entry->getField("valueId")->getValue()+") fitting deviceValue found: " + valueView->fieldId->getValue());
                                                                                    valueView->bindToKnxValue(entry);
                                                                                    return;
                                                                                }
                                                                            }

                                                                            log.warn("knxValue("+entry->getField("valueId")->getValue()+") fitting deviceValue not found");
                                                                        })};

    // define key fields of table
    // first key is AUTOINCREMENT
    SqlEntry::keyFields = { //      tableName       autoincrement   keyFields
            new SqlEntry::KeyFields("container",    "id",           {"id"}),
            new SqlEntry::KeyFields("deviceValues", "id",           {"id"}),
            new SqlEntry::KeyFields("knxValues",                    {"name", "valueId"})
    };





    // -- create tables if not exists
    string sql =    "CREATE TABLE IF NOT EXISTS deviceValues(" \
                    "id         INTEGER PRIMARY KEY AUTOINCREMENT," \
                    "name       TEXT," \
                    "type       TEXT," \
                    "knxIn      TEXT," \
                    "knxOut     TEXT," \
                    "deviceId   INTEGER," \
                    "value      TEXT,"
                    "parentId   INTEGER" \
                    ");" \

                    "CREATE TABLE IF NOT EXISTS knxValues(" \
                    "name       TEXT,"
                    "valueId    INTEGER," \
                    "knxAddr    TEXT," \
                    "value      TEXT" \
                    ");" \

                    "CREATE TABLE IF NOT EXISTS container( " \
                    "id         INTEGER PRIMARY KEY AUTOINCREMENT," \
                    "name       TEXT," \
                    "parentId   INTEGER" \
                    ");";


    // execute
    char *err;
    ok = (sqlite3_exec(database, sql.c_str(), NULL, NULL, &err) == SQLITE_OK);

    string errStr = ok ? "" : string(err);
    log.out("create tables (if not exists)", !ok, errStr);

    // insert
    sql = "BEGIN TRANSACTION;\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',14,'12/12/8','0');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',17,'12/12/9','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',18,'12/12/7','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',19,'','0');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',16,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',15,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',20,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',21,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',22,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',23,NULL,'0');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',25,NULL,'1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',24,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',26,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',27,NULL,'0');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',28,'','0');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',29,NULL,'1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('out',30,'','1');\n"
            "INSERT INTO `knxValues` (name,valueId,knxAddr,value) VALUES ('in',14,'12/0/1','0');\n"
            "\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (14,'Licht 1','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (15,'Schalter','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (16,'Wohnzimmer Licht','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (17,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (18,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (19,'lich,,t6','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (20,'lich,,t6','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (21,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (22,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (23,'lich,,t6','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (24,'lich,,t6','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (25,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (26,'lich,,t6','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (27,'lich,,t6','switch','0',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (28,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (29,'lich,,t6','switch','1',6);\n"
            "INSERT INTO `deviceValues` (id,name,type,value,parentId) VALUES (30,'lich,,t6','switch','1',6);\n"
            "\n"
            "INSERT INTO `container` (id,name,parentId) VALUES (1,': 1',-1);\n"
            "INSERT INTO `container` (id,name,parentId) VALUES (6,': 6 - parent 1',1);\n"
            "INSERT INTO `container` (id,name,parentId) VALUES (7,': 7 - parent 1',1);\n"
            "INSERT INTO `container` (id,name,parentId) VALUES (8,': 8 - parent 5',5);\n"
            "INSERT INTO `container` (id,name,parentId) VALUES (9,': 9 - parent 7',7);\n"
            "INSERT INTO `container` (id,name,parentId) VALUES (12,': 10 - parent 10',1);\n"
            "COMMIT;";

    // execute
    /*
    ok = (sqlite3_exec(database, sql.c_str(), NULL, NULL, &err) == SQLITE_OK);

    errStr = ok ? "" : string(err);
    log.out("insert test entrys ", !ok, errStr);
     */

    // load
    dbLoadElements();

    // find parents for container
    for (auto container : containers)
    {
        container->updateParent();
    }

    log.debug("find parents for deviceValues ...");
    // find parents for deviceValues
    for (auto deviceValue : deviceValues)
    {
        deviceValue->updateParent();
    }
    log.ok("find parents for deviceValues");

    // find deviceValues for knxValues
    for (auto deviceValue : deviceValues)
    {
        deviceValue->updateKnxValues();
    }
};


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
                    sqlEntryOld->inLocalDb = true;
                    sqlEntryOld->fromSql(stmt);
                }
                else
                {
                    // perform action, add new
                    sqlEntryNew->inLocalDb = true;
                    SqlEntry::onNewEntryOfTable(tableName, sqlEntryNew);
                    sqlEntrys.push_back(sqlEntryNew);
                    //sqlEntryNew->syncPush();
                }

            }
            sqlite3_finalize(stmt);
        }
    }
    sqlite3_finalize(stmtListTables);
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


// -- SQL QUERY -------------------------
bool Save::sqlQuery(string sql)
{
    char *err;
    bool ok = (sqlite3_exec(database, sql.c_str(), NULL, NULL, &err) == SQLITE_OK);

    if (!ok)
    {
        string errStr = string(err);
        log.err("execute sql '" + sql + "'  err: '" +errStr+ "'");
    }
    else
        log.ok("execute sql '" +sql+ "'");
    return ok;
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
    log.out("close database ", !ok, "");
}