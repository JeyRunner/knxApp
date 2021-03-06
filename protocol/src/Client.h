/* 
 * File:   Connection.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Client CLASS
 * connect to home server
 * send, receive data
 * --------------------------------------
 */


#ifndef _KNX_CONNECTION_H_
#define _KNX_CONNECTION_H_

// include
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
#include "log/Log.h"
#include "Packet.h"
#include "PacketFactory.h"

using namespace std;
namespace home
{

/* Client class
 */
class Client : protected Log
{
    public:
        Client(string ip, int port);
        ~Client();

        // var
        string serverIp;
        int serverPort;

        // init openssl
        void init();

        bool connectToServer();
        bool disconnect();
        bool connected;


        // send Bytes (decrypt them)
        bool sendBytes(char buffer[], int len);

        // send packet
        bool send(Packet *packet);

        // receive Bytes (encrypt them)
        bool receiveBytes(char buffer[], int len);

        // on receive DataPacket
        void onReceive(function<void(home::Packet *packet)> onReceive);

        // message listeners
        void onError(function<void(string msg)> onError);
        void onDisconnect(function<void ()> onDisconnect);

        // show openssl certificates
        void printCertificates(SSL *ssl);


    private:
        // listener
        function<void(string msg)> error;
        function<void(home::Packet* packet)> onReceiveFunc;
        function<void()> onDisconnectFunc;

        // var
        char        recBuff[20];
        sockaddr_in serverAddr;
        hostent    *host;
        int         sock, lenCli;

        // ssl
        SSL_CTX  *ctx;
        SSL      *ssl;

        // receive thread
        bool startReceiveThread();
        static void* thread_receive(void*connection); // thread function
        pthread_t    thread_receive_id;               // thread id
        void receiveLoop();                           // receive loop function -> run in thread_receive
};


};     /* END NAMESPACE */
#endif /* _KNX_CONNECTION_H_ */
