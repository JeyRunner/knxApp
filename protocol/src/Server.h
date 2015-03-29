/* 
 * File:   Server.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Server CLASS
 * home server
 * accept and manage connections
 * --------------------------------------
 */


#ifndef _KNX_SERVER_H_
#define _KNX_SERVER_H_

// include
#include <list>
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
#include <iostream>
#include <string>
#include <stdlib.h>
#include <functional>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Packet.h"
#include "PacketFactory.h"

using namespace std;
namespace home
{
// pre def host
struct Host;

/* Server class
 */
class Server
{
    public:
        Server(int port);
        ~Server();

        // var
        bool connected;

        // init openssl
        void init();

        // start server
        bool start();

        // on receive DataPacket
        void onReceive(function<void(home::Packet *packet, home::Host *client)> onReceive);

        // send Packet to client
        void send(home::Packet *packet, home::Host *client);

        // message listeners
        void onError(function<void(string msg)> onError);

        // show openssl certificates
        void printCertificates(SSL *ssl);

        // print clients list
        void printClients();


    private:
        // var
        int port;
        sockaddr_in  serverAddr;
        int          sockServer;
        fd_set       socksAll;       // all sockets (clients and server)
        list<Host *> socksClients;   // client sockets
        int          sockMax;        // max socket

        // listener
        function<void(string msg)> error;
        function<void(home::Packet* packet, Host *client)> onReceiveFunc;

        // ssl
        SSL_CTX  *ctx;
        SSL      *ssl;
        bool loadCertificates(string CertiticateFile, string keyFile);

        // receive thread
        bool startReceiveThread();
        static void* thread_receive(void*server); // thread function
        pthread_t    thread_receive_id;               // thread id
        void receiveLoop();                           // receive loop function -> run in thread_receive


};


// server Host
struct Host
{
    Host(int socket, SSL *ssl, string ip);

    int socket;
    SSL *ssl;
    string ip;
};


// cont
static const int SERVER_MAX_RECEIVE_BUFFER_SIZE = 128;

};     /* END NAMESPACE */
#endif /* _KNX_SERVER_H_ */
