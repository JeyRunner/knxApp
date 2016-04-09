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

#include <StyleRule.h>
#include "DeviceValueView.h"
#include "KnxUi.h"


// -- CREATE OBJECT ---------------------
DeviceValueView::DeviceValueView() : Box("", ".val")
{
    setLogName("DVAL");
    sqlEntry = NULL;

    // create ui
    txtName     = new Text("", ".valName");
    editBox     = new Box();
    editTxtName = new Text("", ".valEditName");

    addChild(txtName);
    editBox->addChild(editTxtName);

    // edite me
    txtName->onTouchDown([&](View* v, Point a, Point b, Point c){
        if (KnxUi::modeEdit)
        {
            KnxUi::setEdit(this->editBox);
        }
    });

    Save::deviceValues.push_back(this);
}


// -- BIND TO SQL ENTRY ----------------
void DeviceValueView::bindToSqlEntry(SqlEntry *entry)
{
    this->sqlEntry  = entry;
    this->fieldId   = entry->getField("id");
    this->fieldId   = entry->getField("localId");
    this->fieldName = entry->getField("name");

    entry->setOnChangeField(this);
    txtName->text(fieldName->getValue());
    editTxtName->text(fieldName->getValue());
}

// -- BIND TO KNX VALUE ----------------
void DeviceValueView::bindToKnxValue(SqlEntry *entry)
{
    debug("bind knxValue " + entry->getField("name")->getValue());
    entry->setOnChangeField(this);
    knxValues.push_back(entry);

    // add to edit view
    Box  *knxBox  = new Box();
    knxBox->style->width->setPercent(100);

    Text *knxName = new Text("", ".valTxtKnxEntry");
    Text *knxAddr = new Text("", ".valTxtKnxEntry");
    knxName->text(entry->getField("name")->getValue());
    knxAddr->text(entry->getField("knxAddr")->getValue());
    knxBox->addChild(knxName);
    knxBox->addChild(knxAddr);
    editBox->addChild(knxBox);
}


void DeviceValueView::onChangeField(SqlEntry::Field *field)
{
    if (field->name == "name")
    {
        txtName->text(fieldName->getValue());
        editTxtName->text(fieldName->getValue());
    }

    if (field->name == "parentId")
    {
        warn("parent id changed but there is no code to find new parent :(");
        updateParent();
    }
}



// -- UPDATE PARENT --------------------
void DeviceValueView::updateParent()
{
    if (sqlEntry == NULL)
        return;

    string parentId = sqlEntry->getField("parentId")->getValue();
    string myId     = fieldId->getValue();

    // if not set -> child of root
    if (parentId == "-1")
    {
        debug(myId + ":  added to root contend");
        Save::containerRoot->addContend(this);
        return;
    }


    // find parent
    for (ContainerView *container : Save::containers)
    {

        if (container->sqlEntry != NULL)
        {
            string otherId = container->sqlEntry->getField("id")->getValue();

            if (otherId == parentId)
            {
                // remove from old parent, add to new
                if ((parent != NULL) && (parent != container))
                    parent->removeContend(this);

                if (parent != container)
                    container->addContend(this);

                parent = container;
                break;
            }
        }
    }
}


// -- UPDATE KNX VALUES -----------------
void DeviceValueView::updateKnxValues()
{
    // put to fitting deviceValue
    for (SqlEntry *entry : Save::knxValues)
    {
        string valueId = entry->getField("valueId")->getValue();

        if ( sqlEntry->getField("id")->getValue() == valueId)
        {
            bindToKnxValue(entry);

        }
    }
}

// -- GET KNX VALUE -------------------------
SqlEntry *DeviceValueView::getKnxValue(string name, bool createIfNotExits)
{
    for (auto value : knxValues)
    {
        // debug("search knxValue '"+ name +"' / candidat " + value->getField("name")->getValue());
        if (value->getField("name")->getValue() == name)
            return value;
    }
    warn("requested knxValue not '"+ name +"' found -> create new");

    if (createIfNotExits)
    {
        SqlEntry *value = new SqlEntry("knxValues", 4);
        value->addField(new SqlEntry::Field("valueId", sqlEntry->getField("id")->getValue(), value));
        value->addField(new SqlEntry::Field("name", name, value));
        value->onNewEntryOfTable("knxValues", value);
        return value;
    }

    return nullptr;
}



// ## SWITCH DEVICE VALUE ###################################
// -- CREATE OBJECT ---------------------
SwitchDeviceValue::SwitchDeviceValue() : DeviceValueView::DeviceValueView()
{
    status = false;

    // create ui
    btOn = new Text("", ".valBtOn");
    btOn->onTouchDown([&](View *v, Point relativeSelf, Point relativeParent, Point absolute)
    {
        status = !status;
        setBtOnStatus(status);

        // save to local db
        SqlEntry *out = getKnxValue("out", true);
        out->getField("value")->setValue((status ? "1" : "0"));
        debug(out->getField("value")->getValue());
        out->syncPush();
    });

    addChild(btOn);
    setBtOnStatus(status);
}


// -- BIND TO SQL ENTRY ----------------
void SwitchDeviceValue::bindToSqlEntry(SqlEntry *entry)
{
    DeviceValueView::bindToSqlEntry(entry);

    // status = (knxValueSwitchIn->getField("value")->getValue() == "1" ? true : false);
    setBtOnStatus(status);
}


// -- BIND TO SQL ENTRY ----------------
void SwitchDeviceValue::bindToKnxValue(SqlEntry *entry)
{
    DeviceValueView::bindToKnxValue(entry);

    // update gui
    for (auto field : entry->fields)
    {
        onChangeField(field);
    }
}



void SwitchDeviceValue::onChangeField(SqlEntry::Field *field)
{
    DeviceValueView::onChangeField(field);

    if ((field->entry->tableName == "knxValues") && (field->name == "value"))
    {
        if (field->entry->getField("name")->getValue() == "out")
        {
            status = (field->getValue() == "1" ? true : false);
            setBtOnStatus(status);
        }
    }
}


// -- BT ON ---------------------------
void SwitchDeviceValue::setBtOnStatus(bool status)
{
    if (status)
    {
        btOn->text("an");
        btOn->style->backgroundColor->set("88FF88");
    }
    else
    {
        btOn->text("aus");
        btOn->style->backgroundColor->set("FF8888");
    }
}