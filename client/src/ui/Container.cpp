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
Container::Container() : Box::Box("", ".cont")
{
    setLogName("CONT");
    sqlEntry = NULL;

    // create ui
    txtName = new Text("", ".contTxt");
    boxContend = new Box("", ".contCont");

    addChild(txtName);
    addChild(boxContend);

    Save::containers.push_back(this);
}


// -- CONTEND ---------------------------
void Container::addContend(Container *view)
{
    boxContend->addChild(view);
}

void Container::removeContend(Container *view)
{
    if (dynamic_cast<View*>(view))
        boxContend->removeChild(dynamic_cast<View*>(view));
}


// -- SQL ENTRY ------------------------
void Container::onChangeField(SqlEntry::Field *field)
{
    if (field->name == "name")
    {
        txtName->text(field->getValue());
    }

    if (field->name == "parentId")
    {
        warn("parent id changed but there is no code to find new parent :(");
    }
}


// -- BIND TO SQL ENTRY ----------------
void Container::bindToSqlEntry(SqlEntry* entry)
{
    this->sqlEntry = entry;
    entry->setOnChangeField(this);
    txtName->text(sqlEntry->getField("name")->getValue());
}


// -- UPDATE PARENT --------------------
void Container::updateParent()
{
    if (sqlEntry == NULL)
        return;

    string parentId = sqlEntry->getField("parentId")->getValue();
    string myId     = sqlEntry->getField("id")->getValue();
    string localId  = sqlEntry->getField("localId")->getValue();

    // if not set -> child of root
    if (parentId == "-1")
    {
        trace(localId + ":  added to root contend");
        Save::containerRoot->addContend(this);
        return;
    }


    // find parent
    for (Container *container : Save::containers)
    {

        if (container->sqlEntry != NULL)
        {
            string otherId = container->sqlEntry->getField("id")->getValue();
            string otherLocalId = container->sqlEntry->getField("localId")->getValue();

            if (((otherId != "-1") && (otherId == parentId))
                || ((otherId == "-1") && (otherLocalId != "-1") && (otherLocalId == parentId)))
            {
                // remove from old parent, add to new
                //if ((parent != NULL) && (parent != element))
                // parent->removeContend(this);

                if (parent != container)
                    container->addContend(this);

                parent = container;
                break;
            }
        }
    }
}





// -- UPDATE TEXT ----------------------
/*
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
 */