/* 
 * File:   SqlElement.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * SqlElement CLASS
 * - element that can be stored in a sql database
 * --------------------------------------
 */

#include "SqlElement.h"
#include "Save.h"

//static int        SqlElement::tablesAmount   = 2;


// -- CREATE OBJECT ---------------------
SqlElement::SqlElement()
{
    fieldsAmount = 6;
    tableName    = "";

    id           = -1;
    localId      = -1;
    parentId     = -1;
    changeNr     = -1;
    uploaded     = false;
    time         = 0;
}

// -- UPDATE ALL ------------------------
void SqlElement::updateAll()
{
    updateParentId(parentId);
}

// -- UPDATE PARENTID -------------------
void SqlElement::updateParentId(int id)
{
    parentId = id;

    // if not set -> child of root
    if (parentId < 0)
    {
        cout << "[SQLE] '"<< localId <<"' added to root contend" << endl;
        Save::containerRoot->addContend(this);
        return;
    }


    // find parent
    for (SqlElement *element : Save::sqlElements)
    {
        if (   ( (element->id > -1) && (element->id == parentId) )
            || ( (element->id <  0) && (element->localId > -1) && (element->localId == parentId) ))
        {
            // remove from old parent, add to new
            //if ((parent != NULL) && (parent != element))
                // parent->removeContend(this);

            if (parent != element)
                element->addContend(this);

            parent = element;
            break;
        }
    }
}

void SqlElement::addContend(SqlElement *view) { }
void SqlElement::removeContend(SqlElement *view) { }



// == TABLES ===================================
string SqlElement::getTableName(int tableKey)
{
    switch (tableKey)
    {
        case SQL_TABLE_CONTAINER:
            return "container";

        case SQL_TABLE_CONTAINER_VALUES:
            return "containerValues";
    }
}

SqlElement* SqlElement::createSqlElement(int tableKey)
{
    switch (tableKey)
    {
        case SQL_TABLE_CONTAINER:
            return new Container();

        case SQL_TABLE_CONTAINER_VALUES:
            return NULL;
    }
}


// == GET FIELD BY NAME ========================
bool* SqlElement::getFieldBool(int key)
{
    switch (key)
    {
        case SQL_FIELD_UPLOADED:
            return &uploaded;

        default:
            return nullptr;
    }
}

string SqlElement::getField(int key)
{
    bool   *varB = getFieldBool(key);
    int    *varI = getFieldInt(key);
    string *varS = getFieldString(key);

    if (varB != nullptr)
        return to_string(*varB);

    if (varI != nullptr)
        return to_string(*varI);

    if (varS != nullptr)
        return *varS;
}

int* SqlElement::getFieldInt(int key)
{
    switch (key)
    {
        case SQL_FIELD_ID:
            return &id;

        case SQL_FIELD_LOCAL_ID:
            return &localId;

        case SQL_FIELD_PARENT_ID:
            return &parentId;

        case SQL_FIELD_CHANGE_NR:
            return &changeNr;

        case SQL_FIELD_TIME:
            return &time;

        default:
            return nullptr;
    }
}

string* SqlElement::getFieldString(int key)
{
    return nullptr;
}

int SqlElement::getField_Key(string name)
{
    if (name == "id")
        return SQL_FIELD_ID;

    if (name == "localId")
        return SQL_FIELD_LOCAL_ID;

    if (name == "parentId")
        return SQL_FIELD_PARENT_ID;

    if (name == "changeNr")
        return SQL_FIELD_CHANGE_NR;

    if (name == "uploaded")
        return SQL_FIELD_UPLOADED;

    if (name == "time")
        return SQL_FIELD_TIME;

    return -1;
}

string SqlElement::getField_Name(int key)
{
    switch (key)
    {
        case SQL_FIELD_ID:
            return "id";

        case SQL_FIELD_LOCAL_ID:
            return "localId";

        case SQL_FIELD_PARENT_ID:
            return "parentId";

        case SQL_FIELD_CHANGE_NR:
            return "changeNr";

        case SQL_FIELD_UPLOADED:
            return "uploaded";

        case SQL_FIELD_TIME:
            return "time";

        default:
            return "";
    }
}