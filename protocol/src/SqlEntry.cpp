/* 
 * File:   SqlEntry.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * SqlEntry CLASS
 * - entry that can be stored in a sql database
 * --------------------------------------
 */

#include "SqlEntry.h"

// static
vector<SqlEntry::GenerateIdForTable*> SqlEntry::generateIdForTableList;


// -- CREATE OBJECT ---------------------
SqlEntry::SqlEntry(string tableName, int fieldsAmount)
{
    this->onChangeField = NULL;
    this->fieldsAmount = fieldsAmount;
    this->tableName    = tableName;
}

// -- FIELD ------------------------------
SqlEntry::Field *SqlEntry::getField(string name)
{
    for (auto field : fields)
    {
        if (field->name == name)
            return field;
    }
    return nullptr;
}

void SqlEntry::addField(SqlEntry::Field *field)
{
    fields.push_back(field);
}

void SqlEntry::removeField(SqlEntry::Field *field)
{
    fields.remove(field);
}


// -- ON CHANGE FIELD -------------------
void SqlEntry::setOnChangeField(SqlEntry::OnChangeField *onChangeField)
{
    this->onChangeField = onChangeField;
}


// -- CONVERT SQL -----------------------
void SqlEntry::fromSql(sqlite3_stmt *stmt)
{
    int cols = sqlite3_column_count(stmt);
    string col_name = "";
    string col_value = "";

    for (int i = 0; i < cols; i++)
    {
        if (sqlite3_column_name(stmt, i))
            col_name = const_cast<const char *>(sqlite3_column_name(stmt, i));

        if (sqlite3_column_text(stmt, i))
            col_value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));


        // fill in
        // @TODO fromSql: create different fields by type
        SqlEntry::Field *fieldOld= getField(col_name);

        if (fieldOld != nullptr)
            fieldOld->setValue(col_value);
        else
        {
            addField(new Field(col_name, col_value, this));
            //cout << tableName << " create field " << col_name << ": " << col_value << endl;
        }
    }

    for (auto field : fields)
    {
        //cout << tableName << " has field " << field->name << ": " << field->getValue() << endl;
    }
}

string SqlEntry::toSql()
{
    // @TODO sqlEntry to sql
    return string();
}

// -- GENERATE ID -----------------------
string SqlEntry::getUniqueID()
{

    for (auto endo : generateIdForTableList)
    {
        if (endo->tableName == tableName)
            return endo->functionDo(this);
    }

}


// -- CONVERT JSON ----------------------
void SqlEntry::fromJSON()
{
    // @TODO sqlEntry from JSON
}

string SqlEntry::toJSON()
{
    // @TODO sqlEntry to JSON
    return string();
}


// -- SAVE TO SQL - SYNC TO SERVER -----
void SqlEntry::syncPush()
{
    // @TODO sqlEntry SAVE TO SQL - SYNC TO SERVER
}

