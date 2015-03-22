/* 
 * File:   main.cpp
 * Author: Joshua Johannson
 *
 * main
 */

// include
#include <unistd.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <Ui.h>
#include <Text.h>
#include <Box.h>

using namespace std;


// var
const string serverIp = "192.168.1.164";
const int serverPort = 3671;

// ui
Ui *ui;
Box *root;
Text *headline;


/*
 * main
 */
int main(int argc, char **argv)
{
    // start
    cout << "[ OK ] start program" << endl;

    // init ui
    Ui::init();


    // ui components
    root     = new Box();
    headline = new Text();

    headline->text("Knx");
    root->addChild(headline);


    // open screen
    ui = new Ui(500, 600, "Knx Client");
    ui->setRootView(root);
    ui->frameRenderer->start();


    // @TODO setup homeserver connection


    // not close programm
    while (true)
    {
        sleep(100000);
    }
}