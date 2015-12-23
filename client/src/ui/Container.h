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


#ifndef KNX_CONTAINER_H
#define KNX_CONTAINER_H

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include <Box.h>
#include <Text.h>
#include "../Save.h"
#include "../SqlElement.h"

using namespace std;


/* Container class
 */
class Container : public SqlElement, public Box
{
    public:
        string name;

        Container();
        Box   *boxContend;

        virtual void updateAll();       // after change in sql
        void setName(string text);      // set name
        void updateName(string text);   // after change in sql
        void addContend(SqlElement *view);    // add container/deviceValue
        void removeContend(SqlElement *view); // remove container/deviceValue

        virtual int*    getFieldInt(int key);      // get sql field by key
        virtual string* getFieldString(int key);   // get sql field by key
        virtual bool*   getFieldBool(int key);     // get sql field by key
        virtual string  getField_Name(int key);    // get sql field name by int key
        virtual int     getField_Key(string name); // get int key by field name

    private:
        Text  *txtName;
};

// const
static const char SQL_FIELD_NAME      = 6;

#endif /* KNX_CONTAINER_H */
