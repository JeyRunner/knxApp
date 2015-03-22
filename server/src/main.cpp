/*
 * File:   main.cpp
 * Author: Joshua Johannson
 *
 * knx server daemon
 */

// include
// net
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include "knx/TunnelingRequest.h"
#include "knx/KnxConnection.h"
#include "knx/DataPacket.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <string>
#include <stdlib.h>
using namespace std;


// knx ip router
const string    serverIp   = "192.168.1.164";
const int       serverPort = 3671;

// knx connection
KnxConnection  *knx;

// general
const char DAEMON_NAME[] = "KnxServer";

// progress
pid_t progChild, idChild;



// ## START DAEMON ####################
int main(int argc, char** argv)
{
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



    // -- MAIN ---- ---------
    // create knx connection
    KnxConnection knx(serverIp, serverPort);


    // set error output
    knx.onError([&](string msg)
    {
        syslog (LOG_NOTICE, msg.c_str());
    });

    // connect
    if (knx.connect())
        syslog (LOG_NOTICE, "[MAIN] connected to knx router [OK]");
    else
        syslog (LOG_ERR, "[MAIN] connected to knx router [ERR]");


    while (true)
    {
        sleep(10000);

    }

    // -- END  LOOP ---------


    // close Log
    closelog();
}
