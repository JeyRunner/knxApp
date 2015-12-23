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
#include <Log.h>
#include <Text.h>
#include <Box.h>
#include <Style.h>
#include <Log.h>
#include "ui/Alert.h"
#include "ui/Container.h"
#include "Save.h"
#include "../../protocol/src/PacketFactory.h"
#include "../../protocol/src/Packet.h"
#include "../../protocol/src/Client.h"
#include "../../protocol/src/to_string.cpp"


using namespace std;


// var
const string    SERVER_IP   = "192.168.1.170";
const int       SERVER_PORT = 5049;
home::Client *client;

// ui
Ui *ui;
Box *root;
Text *headline;
Text *out;
Alert *alert;
Log logm;

// pre def
void onError(string msg);
void onDisconnect();
void onReceive(home::Packet *packet);


/*
 * main
 */
int main(int argc, char **argv)
{
    // start
    logm.setLogName("MAIN");
    logm.ok("start program");

    // init ui
    Ui::init();
    Log::setLogLevel(UI_LOG_LEVEL_DEBUG);


    // ui components
    root     = new Box();
    headline = new Text();
    out      = new Text();
    alert    = new Alert(root);
    Save::containerRoot = new Container();

    headline->text("Knx");
    root->addChild(headline);
    root->addChild(Save::containerRoot);
    //root->addChild(out);

    // style
    Style::getRule("*")->textSize->set(30);

    StyleRule stCont(".cont");
    StyleRule stContTxt(".contTxt");
    StyleRule stContCont(".contCont");
    stCont.backgroundColor->set("#33B5E5");
    stContTxt.textColor->set("#FFFFFF");
    stContTxt.paddingBottom->set(5);
    stContTxt.paddingLeft->set(5);
    stContTxt.paddingRight->set(5);
    stCont.top->set(5);
    stCont.left->set(5);
    stCont.right->set(5);
    stCont.bottom->set(5);
    stContCont.paddingLeft->set(15);
    stContCont.paddingRight->set(5);
    stContCont.paddingTop->set(5);
    stContCont.paddingBottom->set(5);
    stContCont.backgroundColor->set("#FFFFFF");
    stContCont.left->set(2);
    stContCont.bottom->set(2);
    stContCont.right->set(2);

    // in android other font path
    #ifdef pl_andr
    Style::getRule("*")->textFamily->set("FreeSans.ttf");
    Style::getRule("*")->textSize->set(50);
    #endif

    // open screen
    ui = new Ui(500, 600, "Knx Client");
    ui->setRootView(root);
    ui->frameRenderer->start();


    // init database

    Save::init();

    /*
    Container *container = new Container();
    container->parentId = 7;
    Save::dbAddNewElement(container);
    container->setName(": 9 - parent 7");

    Container *container2 = new Container();
    container2->parentId = 9;
    Save::dbAddNewElement(container2);
    container2->setName(": 10 - parent 9");
    */

    // setup home server connection
    client = new home::Client(SERVER_IP, SERVER_PORT);
    client->onError(&onError);
    client->onReceive(&onReceive);
    client->onDisconnect(&onDisconnect);

    // init
    client->init();

    // connect
    client->connectToServer();


    // not close programm
    while (true)
    {
        sleep(100000);
    }
}



// -- ON RECEIVE ------------------------
void onReceive(home::Packet *packet)
{
    logm.info("receive packet '" + str(packet->type) + "'");

    // cast to knx packet
    home::KnxPacket *clientPacket = ((home::KnxPacket*)packet);

    // if knx packet
    if (clientPacket)
    {
        // get data from home packet
        int main, middle, sub,
                area, groupe, line;
        clientPacket->getDestinationAddr(main, middle, sub);
        clientPacket->getSourcAddr(area, groupe, line);

        // value
        string value = "UNKNOWN VALUE";
        if (((SwitchValue*)clientPacket->value))
        {
            value = ((SwitchValue*)clientPacket->value)->get() ? "on" : "off";
        }

        out->text(out->text() + "[GET ] src: " + to_string(main) + "/" + to_string(middle) + "/" + to_string(sub) + " (" + value + ")\n");
    }
}


// -- ON ERROR --------------------------
void onError(string msg)
{
    cout << msg << endl;

    // show alert
    alert->show(msg);
}


// -- ON DISCONNECT ---------------------
void onDisconnect()
{
    logm.err("disconnected");

    //alert
    Text *button = new Text();
    button->text("Reconnect");
    button->onTouchDown([&](View *v, Point relativeSelf, Point relativeParent, Point absolute)
    {
        alert->hide();

        // reconnect
        client->connectToServer();
    });

    alert->show("no connection to server", button);
}