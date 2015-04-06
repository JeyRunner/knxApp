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
    const SSL_METHOD  *method;

    // init lib
    SSL_library_init();

    OpenSSL_add_all_algorithms();   // load cryptos...
    SSL_load_error_strings();       // register error
    method = SSLv3_client_method(); // Create new client-method instance
    ctx = SSL_CTX_new(method);      // create new context
    if (ctx == NULL)
    {
        error("[CLIE] ssl ctx new [ERR]");
        // ERR_print_errors(stderr);
    }
}


// -- CONNECT TO SERVER -----------------
bool Client::connectToServer()
{
    cout << "[CLIE] connect to server [...]" << endl;

    // check host ip
    host = gethostbyname(serverIp.c_str());
    if (host == NULL)
    {
        error("server ip '" + serverIp + "' dose not exists");
        return false;
    }
    else
        cout << "[CLIE] ip '" + string(host->h_name) + "' exists [OK]" << endl;


    // server address
    serverAddr.sin_family = host->h_addrtype;
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    serverAddr.sin_port = htons(serverPort);


    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        error("open socket to server");
        perror("[CLIE] open socket [ERR]");
        return false;
    }


    // connect to server
    int r = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)); //bind(sockServer, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    if (r < 0)
    {
        error("can not connect socket to server");
        onDisconnectFunc();
        perror("[CLIE] connect socket to server [ERR]");
        return false;
    }
    else
    {
        cout << "[CLIE] connect [OK]" << endl;
    }


    // ssl -------------------------
    ssl = SSL_new(ctx);     // create new ssl connection
    SSL_set_fd(ssl, sock);  // attach socket descriptor

    // ssl connect
    if ( SSL_connect(ssl) < 0)
    {
        error("can not create ssl connection");
        // ERR_print_errors(stderr);
        disconnect();
        return      false;
    }
    else
    {
        cout << "[CLIE] ssl connect [OK]" << endl;
    }

    // show certificates from server
    printCertificates(ssl);

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

    // close ssl
    SSL_free(ssl);

    // close socket
    close(sock);

    // event
    onDisconnectFunc();
}



// -- START RECEIVE THREAD ------------------
bool Client::startReceiveThread()
{
    int result;
    result = pthread_create(&thread_receive_id /* get thread id */,
                            NULL               /* thread params => default */,
                            &thread_receive    /* thread function */,
                            this               /* transpher classes to thread */);
    // check if successful
    switch (result)
    {
        case 0:
            cout << "[CLIE] create receive thread [OK] \n";
            break;

        default:
            error("can not create receive thread");
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
    if (receiveBytes(buffer, 30))
    {
        // get packet
        packet = PacketFactory::createPacket(buffer);

        // call event
        onReceiveFunc(packet);
    }
}



// ----------------------------------------------------------------------------------
// -- SEND RECEIVE -----------------------------------
bool Client::sendBytes(char buffer[], int len)
{
    int r = SSL_write(ssl, buffer, len);

    // error
    if (r < 0)
    {
        char errBuff[256];
        int err = SSL_get_error(ssl, r);
        ERR_error_string(ERR_get_error(), errBuff);

        error("can not write to server via ssl'" + string(errBuff) + "' (" + to_string(err) + ")");
        disconnect();
    }

    // all fine
    else
    {
        return true;
    }
}

bool Client::receiveBytes(char buffer[], int len)
{
    // clear buffer
    memset(buffer, 0, len);

    int numBytes = SSL_read(ssl, buffer, len);

    // error
    if (numBytes < 0)
    {
        char errBuff[256];
        int err = SSL_get_error(ssl, numBytes);
        ERR_error_string(ERR_get_error(), errBuff);

        error("can not read from server via ssl '" + string(errBuff) + "' (" + to_string(err) + ")");

        disconnect();
    }


    // if num bytes = 0
    // => server has diconnected
    else if (numBytes == 0)
    {
        // out
        error("server is disconnected");

        disconnect();
        return false;
    }

    // all fine
    else
    {
        return true;
    }
}

// -- SEND PACKET ------------------
bool Client::send(Packet *packet)
{
    char buffer[packet->length];
    packet->toBytes(buffer);
    sendBytes(buffer, packet->length);
}



// -- SHOW SSL CERTIFICATES -----------------
void Client::printCertificates(SSL *ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("[CLIE] Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("[CLIE] Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("[CLIE] Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("[CLIE] No certificates. [ERR] \n");
}




// -- SET ON ERROR -----------------
void Client::onError(function<void(string msg)> onError)
{
    this->error = onError;
}

// -- SET ON DISCONNECT -----------------
void Client::onDisconnect(function<void()> onDisconnect)
{
    this->onDisconnectFunc = onDisconnect;
}

// -- SET ON RECEIVE ---------------
void Client::onReceive(function<void(Packet *packet)> onReceive)
{
    this->onReceiveFunc = onReceive;
}

// -- DESTROY OBJECT --------------------
Client::~Client()
{
    // close socket
    close(sock);
}