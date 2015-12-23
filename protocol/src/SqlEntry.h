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
#include <list>
#include <functional>
#include <vector>
#include <iostream>
#include <string>
#include <stdlib.h>
#ifdef pl_andr
#include "../../client/android/include/sqlite3.h"
#include <android/asset_manager_jni.h>
#else
#include <sqlite3.h>
#endif

using namespace std;


/* SqlElement class
 */
class SqlEntry
{
    public:
        SqlEntry(string tableName, int fieldsAmount);

        int    fieldsAmount; // number of sql fields
        string tableName;    // name of sql table



        // field
        // template <class type>
        class Field
        {
            public:
                SqlEntry *entry;
                string name;

                Field(string name_, string value_, SqlEntry *entry_)
                {
                    name = name_;
                    value = value_;
                    entry = entry_;
                    if (entry->onChangeField != NULL)
                        entry->onChangeField->onChangeField(this);
                };

                void setValue(string val){
                    string valueOld = value;
                    value = val;
                    if ((entry->onChangeField != NULL) && value != valueOld )
                        entry->onChangeField->onChangeField(this);
                };
                string getValue(){
                    return value;
                };


            protected:
                string value;
        };
        list <Field*> fields;
        Field* getField(string name);
        void addField(Field *field);
        void removeField(Field *field);

        // generate identifire key
        string getUniqueID();


        // on change
        class OnChangeField
        {
            public:
            virtual void onChangeField(Field *field) = 0;
        };
        OnChangeField *onChangeField;
        void setOnChangeField(OnChangeField *onChangeField);


        // convert
        void    fromSql(sqlite3_stmt *stmt); // call after sqlite3_step
        string  toSql();
        void    fromJSON();
        string  toJSON();

        // sync to sever and local sqlDb \n call after change value of field
        void    syncPush();


        struct GenerateIdForTable
        {
            GenerateIdForTable(string table, function<string (SqlEntry *entry)> func)
            { tableName = table; functionDo = func; }
            string tableName;
            function<string (SqlEntry *entry)> functionDo;
        };
        static vector<GenerateIdForTable*> generateIdForTableList;
};

#endif /* KNX_SQLELEMENT_H */
