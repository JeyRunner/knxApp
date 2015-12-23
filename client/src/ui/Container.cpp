/* 
 * File:   Container.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Container CLASS
 * - represents Container entry of database
 * -- room, device, ...
 * - display it as view
 * - has children
 * -- other containers
 * -- DeviceValues
 * --------------------------------------
 */

#include "Container.h"


// -- CREATE OBJECT ---------------------
Container::Container() : Box::Box("", ".cont"), SqlElement()
{
    // sql
    fieldsAmount+= 1;
    tableName    = "container";
    name         = "";
    parentId     = -1;

    // create ui
    txtName = new Text("", ".contTxt");
    boxContend = new Box("", ".contCont");

    addChild(txtName);
    addChild(boxContend);
}


// -- UPDATE TEXT ----------------------
void Container::updateName(string text)
{
    name = text;
    txtName->text(name);
}

void Container::setName(string text)
{
    updateName(text);
    Save::dbUpdateElement(this);
}

// -- UPDATE ALL ------------------------
void Container::updateAll()
{
    SqlElement::updateAll();
    updateName(name);
}


// -- CONTEND ---------------------------
void Container::addContend(SqlElement *view)
{
    if (dynamic_cast<View*>(view))
        boxContend->addChild(dynamic_cast<View*>(view));
}

void Container::removeContend(SqlElement *view)
{
    if (dynamic_cast<View*>(view))
        boxContend->removeChild(dynamic_cast<View*>(view));
}



// == GET FIELD BY NAME ========================
bool* Container::getFieldBool(int key)
{
    bool *val = SqlElement::getFieldBool(key);
    if (val != nullptr)
        return val;

    /*
    switch (key)
    {
        case SQL_FIELD_:
            return &;
    }
    */
}

int* Container::getFieldInt(int key)
{
    int *val = SqlElement::getFieldInt(key);
    if (val != nullptr)
        return val;

    /*
    switch (key)
    {
        case SQL_FIELD_PARENT_ID:
            return &parentId;
    }
    */
}

string* Container::getFieldString(int key)
{
    string *val = SqlElement::getFieldString(key);
    if (val)
        return val;

    switch (key)
    {
        case SQL_FIELD_NAME:
            return &name;

        default:
            return nullptr;
    }
}

int Container::getField_Key(string name)
{
    int val = SqlElement::getField_Key(name);
    if (val > -1)
        return val;

    if (name == "name")
        return SQL_FIELD_NAME;
}

string Container::getField_Name(int key)
{
    string val = SqlElement::getField_Name(key);
    if (!val.empty())
        return val;

    switch (key)
    {
        case SQL_FIELD_NAME:
            return "name";
    }
}
