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
#include <SqlEntry.h>

using ui::Box;
using ui::Text;
using ui::View;
using namespace std;


/* ContainerView class
 */
class ContainerView : public Box, public SqlEntry::OnChangeField
{
    public:
        Box             *boxContend;
        ContainerView   *parent;
        SqlEntry        *sqlEntry;

        ContainerView();
        virtual void bindToSqlEntry(SqlEntry *entry);

        /*
        virtual void updateAll();       // after change in sql
        void setName(string text);      // set name
        void updateName(string text);   // after change in sql
         */
        void updateParent();

        void addContend(View *view);    // add container/deviceValue
        void removeContend(View *view); // remove container/deviceValue

        virtual void onChangeField(SqlEntry::Field *field);

    private:
        Text  *txtName;
};

#endif /* KNX_CONTAINER_H */
