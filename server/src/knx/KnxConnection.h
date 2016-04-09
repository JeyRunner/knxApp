/* 
 * File:   KnxConnection.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * KNXCONNECTION     CLASS
 * connect to knx ip router
 * send, receive data
 * --------------------------------------
 */


#ifndef KNXCONNECTION_H
#define	KNXCONNECTION_H

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

#include <pthread.h>
#include <iostream>
#include <functional>
#include <stdlib.h>
#include <string>
//#include "out.hpp"

class TunnelingRequest;
using namespace std;


class KnxConnection 
{
    public:
        KnxConnection(string ip, int port);
        ~KnxConnection();
        
        bool connect();
        bool disconnect();
        bool connected;
        
        // connect steps
        bool connectServer();
        bool connectionRequest();
        bool connectionStateRequest();
        
        // send receive
        bool sendBytes(char buffer[], int len);
        bool receiveBytes(char buffer[], int len);
        
        // on receive DataPacket
        void onReceiveDataPacket(function<void(TunnelingRequest* packet)> onReceive);
        
        // message listeners
        void onError(function<void(string msg)> onError);
        void onDisconnect(function<void ()> onDisconnect);

        // wait until receive thread ends => receive disconnect response
        void waitDisconnectFinished();

        // var
        string serverIp;
        int    serverPort;
        char   chanelId;
        sockaddr_in clientAddr, serverAddr;

        void printOk(string a);
        void printErr(string a);
        void ab(char buffer[], int len);
        
        
    private:
        // on message
        function<void(string msg)> onErrorFunc;
        function<void()> onDisconnectFunc;
        function<void(TunnelingRequest* packet)> onReceivePackerFunc;
        
        // var
        char        recBuff[20];
        hostent    *host;
        int         sock, lenCli;
        
              
        // receive thread
        bool startReceiveThread();
        static void* thread_receive(void* knxConnection); // thread function
        pthread_t    thread_receive_id;                   // thread id
        void receiveLoop();                                // receive loop function -> run in thread_receive
};

#endif	/* KNXCONNECTION_H */

