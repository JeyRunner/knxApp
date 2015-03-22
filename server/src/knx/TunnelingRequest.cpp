/* 
 * File:   TunnelingRequest.cpp
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * TUNNELINGREQUEST     CLASS
 * tunneling request packet
 * -> tunnel to knx bus
 * --------------------------------------
 */


#include <string>

#include "TunnelingRequest.h"
#include "KnxConnection.h"
#include "KnxPacket.h"
#include "KnxPacket.h"
#include "DataPacket.h"


// static
int TunnelingRequest::counter = 0;

// -- CREATE OBJECT ---------------------
TunnelingRequest::TunnelingRequest(KnxConnection* connection): KnxPacket::KnxPacket(connection)
{
    conHeaderChanelID = &(connection->chanelId);
    
    // default parameter 
    reset();
}

TunnelingRequest::TunnelingRequest(KnxConnection* connection, char bytes[]): TunnelingRequest::TunnelingRequest(connection)
{
    fromBytes(bytes);
}



// -- SET VAR FROM BYTES ----------------
void TunnelingRequest::fromBytes(char bytes[])
{
    // Header
    KnxPacket::fromBytes(bytes);
    
    // Connection Header (4 Bytes)
    conHeaderLenght = bytes[6];
    *conHeaderChanelID = bytes[7];
    conHeaderCounter = bytes[8];
    
    // cEMI-Frame (11 Bytes)
    cemiType         = bytes[10];
    cemiInfoLenght  = bytes[11];
    cemiControl      = bytes[12];
    cemiDRL          = bytes[13];
    cemiAddrSrcHi   = bytes[14];
    cemiAddrSrcLo   = bytes[15];
    cemiAddrDestHi  = bytes[16];
    cemiAddrDestLo  = bytes[17];
}


// -- GET BYTES FROM VAR ----------------
void TunnelingRequest::toBytes(char bytes[]) 
{
    // set counter
    conHeaderCounter = counter;
    
    // Header
    KnxPacket::toBytes(bytes);
    
    // Connection Header (4 Bytes)
    bytes[6] = conHeaderLenght;
    bytes[7] = *conHeaderChanelID;
    bytes[8] = conHeaderCounter;
    bytes[9] = 0x00; // reseved
    
    // cEMI-Frame (11 Bytes)
    bytes[10] = cemiType;
    bytes[11] = cemiInfoLenght;
    bytes[12] = cemiControl;
    bytes[13] = cemiDRL;
    bytes[14] = cemiAddrSrcHi   & 0xff;
    bytes[15] = cemiAddrSrcLo   & 0xff;
    bytes[16] = cemiAddrDestHi  & 0xff;
    bytes[17] = cemiAddrDestLo  & 0xff;
    bytes[18] = 0x01; // 01 data byte following 
    bytes[19] = 0x00; // tpdu
    // bytes[20] = cemiData[0];
    // @TODO use data longer then on byte
    
}



// -- SET GET ---------------------------
void TunnelingRequest::setSourcAddr(int area, int groupe, int line) 
{
    cemiAddrSrcHi = (groupe & 0b00000111) | ((area << 3));
    cemiAddrSrcLo = line;
}

void TunnelingRequest::getSourcAddr(int &area, int &groupe, int &line)
{
    line   = (int)cemiAddrSrcLo;
    groupe = (int)(cemiAddrSrcHi & 0b00000111);
    area   = (int)(cemiAddrSrcHi >> 3);
}


void TunnelingRequest::setDestinationAddr(int main, int middle, int sub) 
{
    cemiAddrDestHi = (middle & 0b00000111) | ((main << 3));
    cemiAddrDestLo = sub;
}

void TunnelingRequest::getDestinationAddr(int &main, int &middle, int &sub)
{
    
    sub    = (int)cemiAddrDestLo;
    middle = (int)(cemiAddrDestHi & 0b00000111);
    main   = (int)(cemiAddrDestHi >> 3);
}


//void TunnelingRequest::setData(char data[], int len) 
//{
//    strncpy(cemiData, data, len);
//}
//
//void TunnelingRequest::getData(char data[], int len) 
//{
//    strncpy(data, cemiData, len);
//}


// -- RESET -----------------------------
void TunnelingRequest::reset() 
{
    // default parameter 
    // when sending the packet
    setType(KNX_PACKET_TUNNELING_REQUEST);
    setTotalLength(21);     // 21 byte
    conHeaderLenght = 0x04; // 4 byte
    cemiType         = 0x11; // message code, 11: Data Service transmitting 
    cemiInfoLenght  = 0x00; // add. info length (0 bytes)
    cemiControl      = 0xbc; // control byte
    cemiDRL          = 0xe0; // DRL byte
    error            = 0x00;
}


// -- SEND ------------------------------
void TunnelingRequest::send() 
{
    KnxPacket::send();
    counter++;
}


// -- GET PACKET BY TYPE ----------------
TunnelingRequest* TunnelingRequest::getTunnelingRequest(char bytes[], KnxConnection* connection)
{
    short type = (short)( bytes[3] | (bytes[2] << 8) );
    
    return new SwitchPacket(connection, bytes);
    // which type
//    switch (0)
//    {
//        case 0:
//            return new SwitchPacket(connection, bytes);
//            
//        default:
//            cout << "[PACK] unknown packet type '"<< type <<"' [ERR]" << endl;
//    }
}