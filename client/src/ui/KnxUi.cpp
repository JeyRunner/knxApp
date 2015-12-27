/* 
 * File:   Ui.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Ui CLASS
 * @...
 * --------------------------------------
 */

#include "KnxUi.h"

// static
bool    KnxUi::modeEdit;
Text    *KnxUi::btEdit;
Box     *KnxUi::boxEdit;
Box     *KnxUi::editing;
Box     *KnxUi::root;


// -- CREATE OBJECT ---------------------
KnxUi::KnxUi()
{
}


// ## GUI ###############################
void KnxUi::setEdit(Box *box)
{
    // remove old
    if (editing != NULL)
    {
        boxEdit->removeChild(editing);
    }
    else
    {
        root->addChild(boxEdit);
    }

    // add new
    boxEdit->addChild(box);
    editing = box;
}

void KnxUi::removeEdit()
{
    // remove old
    if (editing != NULL)
        boxEdit->removeChild(editing);

    editing = NULL;
    root->removeChild(boxEdit);
}