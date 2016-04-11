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


#ifndef KNX_UI_H
#define KNX_UI_H

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include <Box.h>
#include <Text.h>

using ui::Box;
using ui::Text;
using ui::View;
using namespace std;


/* Ui class
 */
class KnxUi
{
    public:
        static bool modeEdit;
        static Text *btEdit;
        static Box  *boxEdit;
        static Box  *editing;
        static Box *root;

        static void setEdit(Box *box);
        static void removeEdit();

        KnxUi();

};


#endif /* KNX_UI_H */
