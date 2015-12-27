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

#ifdef pl_andr
#include "to_string.cpp"
using namespace andr;
#endif
#include "SqlEntry.h"

// static
// vector<SqlEntry::GenerateIdForTable*> SqlEntry::generateIdForTableList;
vector<SqlEntry::KeyFields *>          SqlEntry::keyFields;
function<bool (string sql)>            SqlEntry::sqlQuery;
sqlite3 *                              SqlEntry::database;
vector<SqlEntry::OnNewEntryOfTable*>   SqlEntry::onNewEntryOfTableRules;


// -- CREATE OBJECT ---------------------
SqlEntry::SqlEntry(string tableName, int fieldsAmount)
{
    this->onChangeField = NULL;
    this->fieldsAmount = fieldsAmount;
    this->tableName    = tableName;
    this->inLocalDb    = false;

    this->myKeyFields  = getKeyFieldsOfTable(tableName);
    if (myKeyFields == NULL)
        cout << "[ SQLE ] [  err ] key fields for table '"<< tableName <<"' not found " << endl;
}

// -- FIELD ------------------------------
SqlEntry::Field *SqlEntry::getField(string name, bool creatIfNotExits)
{
    for (auto field : fields)
    {
        if (field->name == name)
            return field;
    }

    if (creatIfNotExits)
    {
        Field *field = new Field(name, "", this);
        addField(field);
        return field;
    }

    return nullptr;
}

SqlEntry::Field *SqlEntry::getField(string name)
{ return getField(name, true); }

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

    for (int i = 0; i < cols; i++)
    {
        string col_name = "";
        string col_value = "";

        if (sqlite3_column_name(stmt, i))
            col_name = const_cast<const char *>(sqlite3_column_name(stmt, i));

        if (sqlite3_column_text(stmt, i))
            col_value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));

        // fill in
        // @TODO fromSql: create different fields by type
        SqlEntry::Field *fieldOld= getField(col_name, false);

        if (fieldOld != nullptr)
            fieldOld->setValue(col_value);
        else
        {
            addField(new Field(col_name, col_value, this));
        }
    }


    /*
    for (auto field : fields)
    {
        cout << tableName << " has field " << field->name << ": " << field->getValue() << endl;
    }
    cout << endl; */
}

string SqlEntry::toSql(bool withKeyFields, bool insertSyntax)
{
    string sql = "";

    // if insert syntax
    if (insertSyntax)
    {
        sql+= " ( ";
    }

    int i = 0;
    for (auto field : fields)
    {
        i++;

        if ((myKeyFields != NULL)  && (!myKeyFields->hasKeyField(field->name)) || withKeyFields)
        {
            sql += "'" + field->name + "'";

            // if update syntax
            if (!insertSyntax)
            {
                sql+= "='" + field->getValue() + "'";
            }

            if (i < fields.size())
                sql+=", ";
        }
    }


    // if insert syntax
    if (insertSyntax)
    {
        sql += ") VALUES (";

        i = 0;
        for (auto field : fields)
        {
            i++;

            //if ((myKeyFields != NULL) && (!myKeyFields->hasKeyField(field->name)))
            {
                sql += "'" + field->getValue() + "'";
                if (i < fields.size())
                    sql += ", ";
            }
        }

        sql+=" )";
    }

    return sql;
}

// -- GENERATE ID -----------------------
string SqlEntry::getUniqueID()
{
    string id = "";
    if (myKeyFields == NULL)
        return id;

    for (string keyField : myKeyFields->keys)
    {
        id+= getField(keyField)->getValue();
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
    // -- send to server, in json
    // @TODO sqlEntry SYNC TO SERVER

    // -- local sql database
    string sql = "";

    // check if already in local sql db
    if (inLocalDb)
    {
        if (myKeyFields == NULL)
            return;
        // update
        sql+= "UPDATE "+ tableName +" SET ";
        sql+= toSql(false, false);

        // where
        sql+=" WHERE ";

        int i = 0;
        bool needAnd = false;
        for (string keyField : myKeyFields->keys)
        {
            i++;
            string value = getField(keyField)->getValue();
            if (value != "")
            {
                if ( needAnd ) //(i < myKeyFields->keys.size())
                    sql += " and ";

                sql += "" + keyField + "='" + value + "'";
                needAnd = true;
            }
        }
        sql+= " ;";

    }
    else
    {
        // insert
        sql+= "INSERT INTO "+ tableName +" ";
        sql+= toSql(true, true);
    }

    // execute sql
    if (sqlQuery != NULL)
    {
        // not already in db and has primary autoincrement
        if (sqlQuery(sql) && !inLocalDb && (myKeyFields->primaryAutoIncrementKey != ""))
        {
            // set id to first key field
            // if not exits -> create new
            Field *field = getField(myKeyFields->primaryAutoIncrementKey);
            field->setValue(to_string(sqlite3_last_insert_rowid(database)));
        }
        inLocalDb = true;
    }

}


// -- KEY FIELDS -----------------------
SqlEntry::KeyFields *SqlEntry::getKeyFieldsOfTable(string tableName)
{
    for (auto item : keyFields)
    {
        if (item->table == tableName)
        {
            return item;
        }
    }
    return nullptr;
}

bool SqlEntry::KeyFields::hasKeyField(string fieldName)
{
    for (auto item : this->keys)
    {
        if (item == fieldName)
            return true;
    }
    return false;
}




// -- GET ACTION BY TABEL NAME ----------
void SqlEntry::onNewEntryOfTable(string tableName, SqlEntry *entry)
{
    for (auto endo : onNewEntryOfTableRules)
    {
        if (endo->tableName == tableName)
            // cout << "---------- fuction do of " << tableName << endl;
            endo->functionDo(entry);
    }
}