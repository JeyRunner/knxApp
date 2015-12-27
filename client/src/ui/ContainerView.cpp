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

#include "ContainerView.h"


// -- CREATE OBJECT ---------------------
ContainerView::ContainerView() : Box::Box("", ".cont")
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
void ContainerView::addContend(View *view)
{
    boxContend->addChild(view);
}

void ContainerView::removeContend(View *view)
{
    if (dynamic_cast<View*>(view))
        boxContend->removeChild(dynamic_cast<View*>(view));
}


// -- SQL ENTRY ------------------------
void ContainerView::onChangeField(SqlEntry::Field *field)
{
    if (field->name == "name")
    {
        txtName->text(field->getValue());
    }

    if (field->name == "parentId")
    {
        warn("parent id changed but there is no code to find new parent :(");
        updateParent();
    }
}


// -- BIND TO SQL ENTRY ----------------
void ContainerView::bindToSqlEntry(SqlEntry* entry)
{
    this->sqlEntry = entry;
    entry->setOnChangeField(this);
    txtName->text(sqlEntry->getField("name")->getValue());
}


// -- UPDATE PARENT --------------------
void ContainerView::updateParent()
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