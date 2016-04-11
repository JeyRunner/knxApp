/* 
 * File:   Alert.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Alert CLASS
 * show alert message
 * --------------------------------------
 */


#ifndef _KNX_ALERT_H_
#define _KNX_ALERT_H_

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include <Box.h>
#include <Text.h>
#include <Style.h>

using ui::Box;
using ui::Text;
using ui::View;
using namespace std;


/* Alert class
 */
class Alert
{
    public:
        Alert(Box *rootView);

        // show
        void show(string msg);
        void show(string msg, View *button);

        // hide
        void hide();


    private:
        // var
        Box  *rootView;
        Box  *rootBox;
        Text *headline;
        Box  *entryBox;
        Text *btOk;
        bool visible;

        float refTextSize;

        // init
        void init();
};


#endif /* _KNX_ALERT_H_ */
