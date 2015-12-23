/* 
 * File:   SqlElement.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * SqlElement CLASS
 * @...
 * --------------------------------------
 */


#ifndef KNX_SQLELEMENT_H
#define KNX_SQLELEMENT_H

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include <Box.h>

using namespace std;


/* SqlElement class
 */
class SqlElement
{
    public:
        SqlElement();
        SqlElement *parent;

        // id
        int id;
        int localId;

        // synchronisation
        int  changeNr;  // sequence change number
        bool uploaded;  // have changed -> need upload
        int  time;      // time of last change - in seconds
        int  parentId;

        int    fieldsAmount; // number of sql fields
        string tableName;    // name of sql table

        virtual void updateAll();                  // after change in sql
        void updateParentId(int id);               // after change in sql
        virtual void addContend(SqlElement *view); // add container/deviceValue
        virtual void removeContend(SqlElement *view); // remove container/deviceValue

        static int         tablesAmount;                   // amount of tables
        static SqlElement* createSqlElement(int tableKey); // create sqlElement by type
        static string      getTableName(int tableKey);

        virtual int*    getFieldInt(int key);      // get sql field by key
        virtual string* getFieldString(int key);   // get sql field by key
        virtual bool*   getFieldBool(int key);     // get sql field by key
        virtual string  getField(int key);         // get sql field by key
        virtual string  getField_Name(int key);    // get sql field name by int key
        virtual int     getField_Key(string name); // get int key by field name
};

// const
static const int  SQL_FIELD_LOCAL_ID    = 0,
                  SQL_FIELD_ID          = 1,
                  SQL_FIELD_PARENT_ID   = 2,
                  SQL_FIELD_CHANGE_NR   = 3,
                  SQL_FIELD_UPLOADED    = 4,
                  SQL_FIELD_TIME        = 5;

static const char SQL_TABLE_CONTAINER        = 0,
                  SQL_TABLE_CONTAINER_VALUES = 1;

#endif /* KNX_SQLELEMENT_H */
