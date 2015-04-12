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
#include <Style.h>
#include "Alert.h"
#include "../../protocol/src/PacketFactory.h"
#include "../../protocol/src/Packet.h"
#include "../../protocol/src/Client.h"


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
    cout << "[ OK ] start program" << endl;

    // init ui
    Ui::init();


    // ui components
    root     = new Box();
    headline = new Text();
    out      = new Text();
    alert    = new Alert(root);

    headline->text("Knx");
    root->addChild(headline);
    root->addChild(out);

    // style
    Style::getRule("*")->textSize->set(30);

    // in android other font path
    #ifdef pl_andr
    Style::getRule("*")->textFamily->set("FreeSans.ttf");
    #endif

    // open screen
    ui = new Ui(500, 600, "Knx Client");
    ui->setRootView(root);
    ui->frameRenderer->start();


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
    cout << "[CLIE] receive packet '" << packet->type << "'" << endl;

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
    cout << "[CLIE] disconnected" << endl;

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