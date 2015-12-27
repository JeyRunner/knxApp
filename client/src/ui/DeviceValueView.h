/* 
 * File:   DeviceValueView.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * DeviceValueView CLASS
 * - represents deviceValue entry of database
 * -- value of device
 * --------------------------------------
 */


#ifndef KNX_DEVICEVALUEVIEW_H
#define KNX_DEVICEVALUEVIEW_H

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include <Box.h>
#include <Text.h>
#include "../Save.h"
#include <SqlEntry.h>
#include "ContainerView.h"

using namespace std;
class ContainerView;


/* DeviceValueView class
 */
class DeviceValueView  : public Box, public SqlEntry::OnChangeField
{
    public:
        SqlEntry        *sqlEntry;
        ContainerView   *parent;
        list<SqlEntry*> knxValues;

        Box             *editBox;

        // fields
        SqlEntry::Field *fieldId;
        SqlEntry::Field *fieldLocalId;
        SqlEntry::Field *fieldName;

        DeviceValueView();
        virtual void bindToSqlEntry(SqlEntry *entry);
        virtual void bindToKnxValue(SqlEntry *entry);

        void updateParent();
        void updateKnxValues();
        SqlEntry* getKnxValue(string name, bool createIfNotExits);

        // sql
        virtual void onChangeField(SqlEntry::Field *field);

    private:
        Text  *txtName;
        Text  *editTxtName;
};


/* SwitchDeviceValue
 */
class SwitchDeviceValue : public DeviceValueView
{
    public:
        // knxValues
        // SqlEntry *knxValueSwitchOut;
        // SqlEntry *knxValueSwitchIn;

        SwitchDeviceValue();
        virtual void bindToSqlEntry(SqlEntry *entry);
        virtual void bindToKnxValue(SqlEntry *entry);

        void setBtOnStatus(bool status);

        // sql
        virtual void onChangeField(SqlEntry::Field *field);

    private:
        bool status;
        Text *btOn;
};

#endif /* KNX_DEVICEVALUEVIEW_H */
