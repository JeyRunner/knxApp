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

#include "Client.h"
using namespace home;


// -- CREATE OBJECT ---------------------
Client::Client(string ip, int port)
{
    this->serverIp = ip;
    this->serverPort = port;
    this->connected = false;
}

// -- INIT ------------------------------
void Client::init()
{

}


// -- CONNECT TO SERVER -----------------
bool Client::connect()
{
    cout << "[... ][client] connect to server" << endl;

    // check host ip
    host = gethostbyname(serverIp.c_str());
    if (host == NULL)
    {
        error("[ERR ][client] ip '" + string(host->h_name) + "' exists");
        return false;
    }
    else
        cout << "[ OK ][client] ip '" + string(host->h_name) + "' exists" << endl;


    // server address
    serverAddr.sin_family = host->h_addrtype;
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    serverAddr.sin_port = htons(serverPort);


    // create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        error("[ERR ][client] open socket");
        return false;
    }


    // client address
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    clientAddr.sin_port = htons(serverPort);


    // bind
    int r = bind(sock, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    if (r < 0)
    {
        error("[ERR ][client] bind socket");
        return false;
    }

    // var
    lenCli = sizeof(clientAddr);

    // start receive loop
    connected = true;
    startReceiveThread();

    return true;
}

// -- DISCONNECT ----------------------------
bool Client::disconnect()
{
    // stop receive loop
    connected = false;

    // close socket
    close(sock);
}



// -- START RECEIVE THREAD ------------------
bool Client::startReceiveThread()
{
    int result;
    result = pthread_create(&thread_receive_id  /*get thread id*/,
            NULL                    /*thread params => default*/,
            &thread_receive       /*thread function*/,
            this                    /*transpher classes to thread*/);
    // check if successful
    switch (result)
    {
        case 0:
            cout << "[ OK ][client] create receive thread \n";
            break;

        default:
            error("[client] create receive thread [ERR] \n");
    }
}


// -- RECEIVE THREAD ------------------------
void *Client::thread_receive(void *connection)
{
    while (((Client *) connection)->connected)
    {
        ((Client *) connection)->receiveLoop();
    }

}

// -- RECEIVE LOOP --------------------------
void Client::receiveLoop()
{
    // var
    char    buffer[30];
    Packet *packet;

    // receive
    receiveBytes(buffer, 30);

    // get packet
    packet = PacketFactory::createPacket(buffer);

    // @TODO check packet type
}



// ----------------------------------------------------------------------------------
// -- SEND RECEIVE -----------------------------------
bool Client::sendBytes(char buffer[], int len)
{
    int r = sendto(  sock, buffer, len, 0,
            (struct sockaddr *) &serverAddr,
            sizeof (serverAddr));
    if(r < 0)
    {
        error("[ERR ][client] send");
        return false;
    }
    else
        return true;

    // @TODO decrypt before send
}

bool Client::receiveBytes(char buffer[], int len)
{
    int lenCli = sizeof (clientAddr);

    // clear buffer
    memset(buffer, 0, len);

    int r = recvfrom( sock, buffer, len /* buffer size */, 0,
            ( sockaddr *) &clientAddr,(socklen_t*) &lenCli );
    if(r < 0)
    {
        error("[ERR ][client] receive ");
        return false;
    }
    else
        return true;

    // @TODO encrypt after receive
}

// -- SEND PACKET ------------------
bool Client::send(Packet *packet)
{
    char buffer[packet->length];
    packet->toBytes(buffer);
    sendBytes(buffer, packet->length);
}



// -- SET ON ERROR -----------------
void Client::onError(function<void(string msg)> onError)
{
    this->error = onError;
}

// -- SET ON RECEIVE ---------------
void Client::onReceive(function<void(Packet *packet)> onReceive)
{
    this->onReceiveFunc = onReceive;
}

// -- DESTROY OBJECT --------------------
Client::~Client()
{
}