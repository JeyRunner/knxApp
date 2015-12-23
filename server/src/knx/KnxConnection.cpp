/* 
 * File:   KnxConnection.cpp
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * KNXCONNECTION CLASS
 * connect to knx ip router
 * send, receive data
 * --------------------------------------
 */

#include "KnxConnection.h"
#include "TunnelingRequest.h"
#include "TunnelingResponse.h"
#include "DisconnectRequest.h"
#include "DisconnectRespose.h"

// -- CREATE OBJECT ------------------
KnxConnection::KnxConnection(string ip, int port) 
{
    this->serverIp   = ip;
    this->serverPort = port;
    this->connected  = false;
}



// -- CONNECT -----------------------
bool KnxConnection::connect() 
{
    bool status;
    
    // bind udp to server
    status = connectServer();
    if (!status)
        return false;
    
    // send connection request
    status = connectionRequest();
    if (!status)
        return false;
    
    // send connection state request
    status = connectionStateRequest();
    if (!status)
        return false;
    
    // successful
    return true;
}



// *****************************************************************************************************
// -- CONNECT STEPS -----------------------------------------------------------------------
bool KnxConnection::connectServer() 
{
    a("\n[NET ] connect to server [...]");
    
    // check host ip
    host = gethostbyname(serverIp.c_str());
    if(host == NULL){
        a("[NET ] ip '"+ string(host->h_name) +"' exists [ERR]");
        return false;
    }   
    else 
        a("[NET ] ip '"+ string(host->h_name) +"' exists [OK]");
    
    
    // server address
    serverAddr.sin_family       = host->h_addrtype;
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    serverAddr.sin_port         = htons(serverPort);
    
    
    // create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock <0) {
        a("[NET ] open socket [ERR]");
        return false;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }
    //else
        //a("[NET ] open socket [OK]");
    
    
    // client address
    clientAddr.sin_family       = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    clientAddr.sin_port         = htons(serverPort);
    
    
    // bind
    int r = bind ( sock, (struct sockaddr *) &clientAddr, sizeof (clientAddr) );
    if(r < 0) {
        a("[NET ] bind socket [ERR]");
        return false;
    }
    //else
        //a("[NET ] bind socket [OK]");
    
    
    // var
    lenCli = sizeof (clientAddr);
    
    return true;
}

// -- CONNECTION_REQUEST ---------------------------------------------------
bool KnxConnection::connectionRequest() 
{
    //a("\n[NET ] -- connection request -------");
    char requ[26];
    
    /* header */
    requ[0] = 0x06; /* 06 - Header Length */
    requ[1] = 0x10; /* 10 - KNXnet version (1.0) */
    requ[2] = 0x02; /* 02 - hi-byte Service type descriptor (CONNECTION_REQUEST) */
    requ[3] = 0x05; /* 05 - lo-byte Service type descriptor (CONNECTION_REQUEST) */
    requ[4] = 0x00; /* 00 - hi-byte total length */
    requ[5] = 0x1A; /* 1A - lo-byte total length 26 bytes */

    /* Connection HPAI */
    requ[6] = 0x08; /* 08 - Host Protocol Address Information (HPAI) Lenght */
    requ[7] = 0x01; /* 01 - Host Protocol Address Information (HPAI) Lenght */
    requ[8] = ntohl(clientAddr.sin_addr.s_addr) >> 24 & 0xff; /* c0 - IP address 00 = 192 */
    requ[9] = ntohl(clientAddr.sin_addr.s_addr) >> 16 & 0xff; /* a8 - IP address 00 = 168 */
    requ[10] = ntohl(clientAddr.sin_addr.s_addr) >> 8 & 0xff; /* 0a - IP address 00 = 10 */
    requ[11] = ntohl(clientAddr.sin_addr.s_addr) & 0xff; /* b3 - IP address 00 = 179 */
    requ[12] = (ntohs(clientAddr.sin_port) >> 8) & 0xff; /* xx - hi-byte local port number for CONNECTION, CONNECTIONSTAT and DISCONNECT requests */
    requ[13] = ntohs(clientAddr.sin_port) & 0xff; /* xx - lo-byte local port number for CONNECTION, CONNECTIONSTAT and DISCONNECT requests */

    /* Tunnelling HPAI */
    requ[14] = 0x08; /* 08 - Host Protocol Address Information (HPAI) Lenght */
    requ[15] = 0x01; /* 01 - Host Protocol Address Information (HPAI) Lenght */
    requ[16] = ntohl(clientAddr.sin_addr.s_addr) >> 24 & 0xff; /* c0 - IP address c0 = 192 */
    requ[17] = ntohl(clientAddr.sin_addr.s_addr) >> 16 & 0xff; /* a8 - IP address a8 = 168 */
    requ[18] = ntohl(clientAddr.sin_addr.s_addr) >> 8 & 0xff; /* 0a - IP address 0a = 10 */
    requ[19] = ntohl(clientAddr.sin_addr.s_addr) & 0xff; /* b3 - IP address 9F = 179 */
    requ[20] = (ntohs(clientAddr.sin_port) >> 8) & 0xff; /* yy - hi-byte local port number for TUNNELLING requests */
    requ[21] = ntohs(clientAddr.sin_port) & 0xff; /* yy - lo-byte local port number for TUNNELLING requests */

    /* CRI */
    requ[22] = 0x04; /* structure len (4 bytes) */
    requ[23] = 0x04; /* Tunnel Connection */
    requ[24] = 0x02; /* KNX Layer (Tunnel Link Layer) */
    requ[25] = 0x00; /* Reserved */
    
    
    sendBytes(requ, 26);
    receiveBytes(recBuff, 20);
    if(recBuff[7] != 0   || !(recBuff[2]==0x02 && recBuff[3]==0x06) /* CONNECTION_RESPONSE */)
    {
        a("[GET ] connection response [ERR]: '"/*+ to_string(recBuff[7]) +"'"*/);
        return false;
    }
    else
        a("[GET ] connection response [OK]");
        
    // get chanel id
    chanelId = recBuff[6];
    a("[INFO] chanel ID: "/*+ to_string(chanelId) +""*/);
    
    return true;
}


/* -- CONNECTIONSTATE_REQUEST --------------------------------------------------------------- */
bool KnxConnection::connectionStateRequest() 
{
    //a("\n[NET ] -- connection state request -------");
    char csreq[16];
    
    /* Header (6 Bytes) */
    csreq[0] = 0x06; /* 06 - Header Length */
    csreq[1] = 0x10; /* 10 - KNXnet version (1.0) */
    csreq[2] = 0x02; /* 02 - hi-byte Service type descriptor (CONNECTIONSTATE_REQUEST) */
    csreq[3] = 0x07; /* 07 - lo-byte Service type descriptor (CONNECTIONSTATE_REQUEST) */
    csreq[4] = 0x00; /* 00 - hi-byte total length */
    csreq[5] = 0x10; /* 10 - lo-byte total lengt 16 bytes */

    /* Connection HAPI (10 Bytes) */
    csreq[6] = chanelId & 0xff; /* zz - given channel id */
    csreq[7] = 0x00; /* 00 - */
    csreq[8] = 0x08; /* 08 - Host Protocol Address Information (HPAI) Lenght */
    csreq[9] = 0x01; /* 01 - Host Protocol Code 0x01 -> IPV4_UDP, 0x02 -> IPV6_TCP */
    csreq[10] = 0x00; //ntohl(clientAddr.sin_addr.s_addr) >> 24 & 0xff; /* c0 - IP address 00 = 192 */
    csreq[11] = 0x00; //ntohl(clientAddr.sin_addr.s_addr) >> 16 & 0xff; /* a8 - IP address 00 = 168 */
    csreq[12] = 0x00; //ntohl(clientAddr.sin_addr.s_addr) >> 8 & 0xff; /* 0a - IP address 00 = 10 */
    csreq[13] = 0x00; //ntohl(clientAddr.sin_addr.s_addr) & 0xff; /* b3 - IP address 00 = 179 */
    csreq[14] = 0x00; //(ntohs(clientAddr.sin_port) >> 8) & 0xff; /* xx - hi-byte local port number for CONNECTION, CONNECTIONSTAT and DISCONNECT requests */
    csreq[15] = 0x00; //ntohs(clientAddr.sin_port) & 0xff; /* xx - lo-byte local port number for CONNECTION, CONNECTIONSTAT and DISCONNECT requests */

    
    //char t[] = { 0x06, 0x10, 0x02 , 0x07 , 0x00 , 0x10 , (chanelId & 0xff) , 0x00 , 0x08 , 0x01 , 0xc0 , 0xa8 , 0x0a , 0xb3 , 0xd9 , 0x6d };
    sendBytes(csreq, 16);
    receiveBytes(recBuff, 8);
    if(recBuff[7] != 0   || !(recBuff[2]==0x02 && recBuff[3]==0x08) /* CONNECTIONSTATE_RESPONSE */)
    {
        a("[GET ] con state response [ERR]: '" /*+to_string(recBuff[7]) +"'"*/);
        return false;
    }    
    else
        a("[GET ] con state response [OK]");
    
    // start receive loop
    connected = true;
    startReceiveThread();
    
    a("");
    return true;
}



// -- START RECEIVE THREAD ------------------
bool KnxConnection::startReceiveThread() 
{
    int result;
    result = pthread_create(&thread_receive_id  /*get thread id*/,
                            NULL                    /*thread params => default*/,
                            &thread_receive       /*thread function*/,
                            this                    /*transpher classes to thread*/);
    // check if successful
    switch(result)
    {
        case 0:
            printf("[NET ] create receive thread [OK] \n");
            break;
            
        default:
            printf("[NET ] create receive thread [ERR] \n");
    }
}


// -- RECEIVE THREAD ------------------------
void *KnxConnection::thread_receive(void* knxConnection)
{
    while ( ((KnxConnection*)knxConnection)->connected ) 
    {
        ((KnxConnection*)knxConnection)->receiveLoop();
    }

}

// -- RECEIVE LOOP --------------------------   
void KnxConnection::receiveLoop() 
{
    // var
    char       buffer[30];
    KnxPacket *packet;
    
    // receive
    receiveBytes(buffer, 30);
    
    // get packet
    packet = KnxPacket::getPacket(buffer, this);
    
    // check packet type
    switch (packet->getType())
    {
        case KNX_PACKET_TUNNELING_REQUEST:
        {
            TunnelingRequest *r = (TunnelingRequest*)packet;
            // int ar, g, l;
            // r->getDestinationAddr(ar,g,l);
            // r->getData(data, 1);
//            ab(buffer, 21);        
            
            // event            
            onReceivePackerFunc(r);
            
            // confirm
            TunnelingResponse(this).send();
            break;
        }
            
        case KNX_PACKET_TUNNELING_RESPONSE:
//            a("[GET ] tunneling response");
            break;


        case KNX_PACKET_DISCONNECT_REQUEST:
        {
            a("[GET ] disconnect request");

            // send disconnect respose
            DisconnectRespose *respose = new DisconnectRespose(this);
            char buffer[respose->getTotalLength()];
            respose->toBytes(buffer);
            sendBytes(buffer, respose->getTotalLength());

            // stop receive loop
            connected = false;

            // close socket
            close(sock);

            onDisconnectFunc();

            break;
        }

        case KNX_PACKET_DISCONNECT_RESPONSE:
            a("[GET ] disconnect response");
            
            // stop receive loop
            connected = false;
            
            // close socket
            close(sock);

            onDisconnectFunc();
            
            break;
            
    }
}





// -- DISCONNECT_REQUEST --------------------   
bool KnxConnection::disconnect() 
{
    // stop receive loop
    connected = false;
    //pthread_cancel(thread_receive_id);
    
    
    a("\n[NET ] disconnect [...]");

    // send disconnect respose
    DisconnectRespose *respose = new DisconnectRespose(this);
    char buffer[respose->getTotalLength()];
    respose->toBytes(buffer);
    sendBytes(buffer, respose->getTotalLength());
    
    return true;
}




// *****************************************************************************************************
// -- SEND RECEIVE -----------------------------------  
bool KnxConnection::sendBytes(char buffer[], int len)
{
    int r = sendto(  sock, buffer, len, 0,
                 (struct sockaddr *) &serverAddr,
                  sizeof (serverAddr));
    if(r < 0)
    {
        a("[NET ] send [ERR]");
        return false;
    } 
    else
    {
        //a("[NET ] send [OK]");
        return true;
    }
}

bool KnxConnection::receiveBytes(char buffer[], int len)
{
    int lenCli = sizeof (clientAddr);
    
    // clear buffer 
    memset(buffer, 0, len);

    int r = recvfrom( sock, buffer, len /* buffer size */, 0,
                   ( sockaddr *) &clientAddr,(socklen_t*) &lenCli );
    if(r < 0)
    {
        a("[NET ] receive [ERR]");
        return false;
    }
    else
    {
        // out
        // ab(buffer, len);
        
        return true;
    }
}







// *****************************************************************************************************
// -- SET ON RECEIVE ---------------
void KnxConnection::onReceiveDataPacket(function<void(TunnelingRequest *packet)> onReceive) 
{
    this->onReceivePackerFunc = onReceive;
}


// -- SET ON ERROR -----------------    
void KnxConnection::onError(function<void(string msg)> onError) 
{
    this->onErrorFunc = onError;
}

// -- SET ON DISCONNECT -----------------
void KnxConnection::onDisconnect(function<void()> onDisconnect)
{
    this->onDisconnectFunc = onDisconnect;
}


// -- OUT --------------------------------------------
void KnxConnection::a(string a)
{
    cout << a << endl;
    
    if (onErrorFunc)
        onErrorFunc(a);
}

void KnxConnection::ab(char c[], int len)
{
    string msg   = "";
    string count = "";
    for(int i=0; i<len; i++)
    {
        msg  += "";/*to_string((char)c[i]) + " : "*/;
        count+= "";/*to_string(i)          + " : "*/;
    }
    a("[GET ] "+ msg +"");
    // a("[GET ] "+ count +"");
}





// -- DESTROY ----------------
KnxConnection::~KnxConnection() 
{
    disconnect();
}
