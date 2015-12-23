/*
 * File:   main.cpp
 * Author: Joshua Johannson
 *
 * knx server daemon
 */

// include
// net
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "knx/TunnelingRequest.h"
#include "save/Save.h"
#include <Server.h>
#include <KnxPacket.h>

#include <syslog.h>
#include <sys/stat.h>
#include <SDL2/SDL_main.h>

using namespace std;


// knx ip router
const string    serverIp   = "192.168.1.164";
const int       serverPort = 3671;

// connections
KnxConnection  *knx;
home::Server   *server;

// general
const char DAEMON_NAME[] = "KnxServer";

// progress
pid_t progChild, idChild;


// pre def
void onKnxReceive(TunnelingRequest *packet);
void onHomeServerReceive(home::Packet *packet, home::Host *client);
void onError(string msg);
void onDisconnect();



// ## START DAEMON ####################
int main(int argc, char** argv)
{
    /*
    //Set our Logging Mask and open the Log
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_LOCAL0);

    // -- start daemon --
    // new child progress
    progChild = fork();

    // bad progress id
    if ( progChild < 0 )
    {
        cerr << "[MAIN] bad progress id" << endl;
        exit(EXIT_FAILURE);
    }


    // good progress id, Close the Parent Process
    if (progChild > 0)
    {
        cout << "[MAIN] good progress id" << endl;
        exit(EXIT_SUCCESS);
        syslog(LOG_NOTICE, "[MAIN] fork successful");
    }



    // change File Mask
    umask(0);

    // create a new Signature Id for our child
    idChild = setsid();

    if (idChild < 0)
    {
        cerr << "[MAIN] create a new Signature Id for our child not work" << endl;
        exit(EXIT_FAILURE);
    }

    // change Directory
    if ((chdir("/")) < 0)
    {
        cerr << "[MAIN] can not change directory to '/'" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "[MAIN] started successful" << endl;

    // close Standard File Descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog(LOG_NOTICE, "[MAIN] started successful");
    */


    // -- MAIN ---- ---------
    // init local database
    Save::init();

    // create knx connection
    knx = new KnxConnection(serverIp, serverPort);
    knx->onReceiveDataPacket(&onKnxReceive);     // on new packet
    knx->onError(&onError);                      // on error
    knx->onDisconnect(&onDisconnect);            // on disconnect
    knx->connect();                              // connect


    // create home server
    server = new home::Server(5049);
    server->onError(&onError);                   // set on error
    server->onReceive(&onHomeServerReceive);     // on new packet
    server->init();                              // start
    server->start();

    /*
    // not close
    while (true)
    {
        sleep(10000);

    }
    */

    // -- END  LOOP ---------

    // close database
    Save::close();

    // close Log
    closelog();
}


// -- KNX RECEIVE -----------------------
void onKnxReceive(TunnelingRequest *packet)
{
    // get data from Tunneling request
    int main, middle, sub,
            area, groupe, line;
    packet->getDestinationAddr(main, middle, sub);
    packet->getSourcAddr(area, groupe, line);


    // transpher data to home server packet
    home::KnxPacket *clientPacket = new home::KnxPacket();
    clientPacket->setDestinationAddr(main, middle, sub);
    clientPacket->setSourcAddr(area, groupe, line);
    clientPacket->value = packet->value;    // transpher data value

    // send to all clients
    for (home::Host *host : server->socksClients)
    {
        server->send(clientPacket, host);
    }
}


// -- HOME RECEIVE ----------------------
void onHomeServerReceive(home::Packet *packet, home::Host *client)
{
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


        // transpher data to home server packet
        TunnelingRequest *knxPacket = new TunnelingRequest(knx);
        knxPacket->setDestinationAddr(main, middle, sub);
        knxPacket->setSourcAddr(area, groupe, line);
        knxPacket->value = clientPacket->value;     // transpher data value

        // send to knx bus
        knxPacket->send();
    }
}


// -- ON ERROR --------------------------
void onError(string msg)
{
    cout << msg << endl;
    //syslog (LOG_NOTICE, msg.c_str());
}

// -- ON DISCONNECT ---------------------
void onDisconnect()
{
    cout << "[KNX ] disconnected" << endl;

    // reconnect
    knx->connect();
}