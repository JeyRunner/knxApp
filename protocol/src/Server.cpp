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

#include "Server.h"
#include <to_string.cpp>

using namespace home;
#ifdef pl_andr
    using namespace andr;
#endif


// -- CREATE OBJECT ---------------------
Server::Server(int port)
{
    this->connected = false;
    this->port = port;
}

// -- INIT ------------------------------
void Server::init()
{
    setLogName("SERV", "HomeServer");

    const SSL_METHOD *method;

    // init lib
    SSL_library_init();

    OpenSSL_add_all_algorithms();   // load cryptos...
    SSL_load_error_strings();       // register error
    method = SSLv3_method();        // Create new client-method instance
    ctx = SSL_CTX_new(method);      // create new context
    if (ctx == NULL)
    {
        // ERR_print_errors(stderr);
        err("ssl ctx new");
    }

    // load certificate
    if (!loadCertificates("/home/joshua/Dokumente/Projekte/knx/server/certificates/knx.crt", "/home/joshua/Dokumente/Projekte/knx/server/certificates/knx.key"))
        exit(EXIT_FAILURE);

}


// -- START ----------------------------
bool Server::start()
{
    // clear address
    bzero(&serverAddr, sizeof(serverAddr));

    // address
    serverAddr.sin_family = AF_INET;;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // create socket (tcp)
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if (sockServer < 0)
    {
        err("create socket");
        connected = false;
        return false;
    }

    // bind
    if (bind(sockServer, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        err("bind socket to port");
        perror("[SERV] bind socket to port [ERR]");
        connected = false;
        return false;
    }

    // listen
    if (listen(sockServer, 10) != 0)
    {
        err("listen");
        perror("[SERV] listen [ERR]");
        connected = false;
        return false;
    }

    // setup fd set
    FD_ZERO(&socksAll);        // clear
    FD_SET(sockServer, &socksAll);   // add server to sockets list
    sockMax = sockServer;

    // connected ok
    connected = true;

    // start main receive loop
    startReceiveThread();
}


// -- START RECEIVE THREAD ------------------
bool Server::startReceiveThread()
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
            ok("create receive thread");
            break;

        default:
            err("create receive thread");
    }
}


// -- RECEIVE THREAD ------------------------
void *Server::thread_receive(void *server)
{
    while (((Server *) server)->connected)
    {
        ((Server *) server)->receiveLoop();
    }

}

// -- RECEIVE LOOP --------------------------
void Server::receiveLoop()
{
    // refill socksAll
    FD_ZERO(&socksAll);
    FD_SET(sockServer, &socksAll);
    for (Host *current : socksClients)
    {
        FD_SET(current->socket, &socksAll);
    }


    // wait for change of one socket
    select(sockMax + 1 /* max socket id + 1 */,
            &socksAll  /* check for something to read */,
            NULL       /* check for something to write */,
            NULL       /* check for error */,
            NULL       /* no timeout */);


    // new on server socket
    // => new connection
    if (FD_ISSET(sockServer, &socksAll))
    {
        // accept new
        int client;
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        SSL *clientssl;

        // accept new client
        client = accept(sockServer, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if (client < 0)
        {
            err("accept new client");
        }
        else
        {
            ok("new client '" + string(inet_ntoa(clientAddr.sin_addr)) + "'");
        }

        // ssl
        clientssl = SSL_new(ctx);       // get new SSL state with context
        SSL_set_fd(clientssl, client);  // set connection socket to SSL state

        // SSL-protocol accept
        if (SSL_accept(clientssl) < 0)
        {
            err("ssl accept");
            ERR_print_errors_fp(stderr);
        }
        else
        {
            // show certificates
            printCertificates(clientssl);
        }

        // add to sockets list
        socksClients.push_back(new Host(client, clientssl, string(inet_ntoa(clientAddr.sin_addr))));

        // update max sock
        if (client > sockMax)
            sockMax = client;

        // show clients
        printClients();
    }


    // check every client
    for (auto current = socksClients.begin(); current != socksClients.end();)
    {
        // if has something to read
        if (FD_ISSET((*current)->socket, &socksAll))
        {
            // data
            char buffer[SERVER_MAX_RECEIVE_BUFFER_SIZE];
            memset(buffer, 0, SERVER_MAX_RECEIVE_BUFFER_SIZE); // clear buffer

            // read Bytes
            int numBytes = SSL_read((*current)->ssl, buffer, SERVER_MAX_RECEIVE_BUFFER_SIZE);


            // error
            if (numBytes < 0)
            {
                char errBuff[256];
                int error = SSL_get_error((*current)->ssl, numBytes);
                ERR_error_string(ERR_get_error(), errBuff);
                err("ssl read '" + string(errBuff) + "' (" + to_string(error) + ")");

                // next client
                current++;
            }


            // if num bytes = 0
            // => client has diconnected
            else if (numBytes == 0)
            {
                // get ip
                string ip = (*current)->ip;

                // erase
                SSL_free((*current)->ssl);
                close((*current)->socket);
                free(*(current));

                // remove from clients list
                socksClients.erase(current++);  // got to next

                // out
                ok("remove client '" + ip + "'");

                // show clients
                printClients();
            }


            // receive message
            else
            {
                // get packet from bytes
                Packet *packet = PacketFactory::createPacket(buffer);

                // call on receive method
                onReceiveFunc(packet, (*current));

                // next client
                current++;
            }
        }

        // if has nothing to read
        else
        {
            // next client
            current++;
        }
    }
}

// -- SEND PACKET --------------------------
void Server::send(home::Packet *packet, Host *client)
{
    char buffer[packet->length];
    packet->toBytes(buffer);
    SSL_write(client->ssl, buffer, packet->length);
}


// -- LOAD SSL CERTIFICATES ----------------
bool Server::loadCertificates(string CertiticateFile, string keyFile)
{
    // set the local certificate from CertFile
    if (SSL_CTX_use_certificate_file(ctx, CertiticateFile.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        err("load local certificate from CertiticateFile");
        ERR_print_errors_fp(stderr);
        return false;
    }
    // set the private key from KeyFile (may be the same as CertFile)
    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        err("load private key from KeyFile");
        ERR_print_errors_fp(stderr);
        return false;
    }
    // verify private key
    if (!SSL_CTX_check_private_key(ctx))
    {
        err("private key does not match the public certificate");
        return false;
    }

    // all ok
    return true;
}

// -- SHOW SSL CERTIFICATES -----------------
void Server::printCertificates(SSL *ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if (cert != NULL)
    {
        info("Server certificates:");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        info("Subject: " + to_string(line));
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        info("Issuer: " + to_string(line));
        free(line);
        X509_free(cert);
    }
    else
        err("No certificates - can not print certificates");
}

// -- PRINT CLIENTS ------------------------
void Server::printClients()
{
    // headline
    info("Clients:");

    for ( Host *current : socksClients )
    {
        info("Client: nr." + to_string(current->socket) + " '" + current->ip + "'");
    }
}



// -- SET ON ERROR -----------------
void Server::onError(function<void(string msg)> onError)
{
    this->error = onError;
}

// -- SET ON RECEIVE ---------------
void Server::onReceive(function<void(home::Packet *packet, Host *client)> onReceive)
{
    this->onReceiveFunc = onReceive;
}

// -- DESTROY OBJECT --------------------
Server::~Server()
{
    // close server
    close(sockServer);  // close server socket
    SSL_CTX_free(ctx);  // release context
}


// -- INIT CLIENT -----------------------
Host::Host(int socket, SSL *ssl, string ip)
{
    this->socket = socket;
    this->ssl = ssl;
    this->ip  = ip;
}