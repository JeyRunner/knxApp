/* 
 * File:   DisconnectRespose.cpp
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * DISCONNECTRESPONSE     CLASS
 * answer to disconnect_request
 * --------------------------------------
 */


#include "DisconnectRespose.h"

// -- CREATE OBJECT ---------------------
DisconnectRespose::DisconnectRespose(KnxConnection* connection): KnxPacket::KnxPacket(connection)
{
    // default
    setType(KNX_PACKET_DISCONNECT_RESPONSE);
    setTotalLength(8);                         // 8 bytes

    conHeaderChanelID = &(connection->chanelId);
}

DisconnectRespose::DisconnectRespose(KnxConnection* connection, char bytes[]): DisconnectRespose::DisconnectRespose(connection)
{
    fromBytes(bytes);
}


void DisconnectRespose::fromBytes(char bytes[]) 
{
     // Header
    KnxPacket::fromBytes(bytes);
    
    
    // Connection Header (4 Bytes)
    conHeaderLenght     = bytes[6];
    *conHeaderChanelID  = bytes[7];
    conHeaderError      = bytes[9];
    
    // set error;
    error = conHeaderError;
}

void DisconnectRespose::toBytes(char bytes[]) 
{
    // Header
    KnxPacket::toBytes(bytes);
    
    // Connection Header (4 Bytes)
    bytes[6] = conHeaderLenght;
    bytes[7] = *conHeaderChanelID & 0xff;
    bytes[8] = 0x01; // 01 data byte following
    bytes[9] = conHeaderError;
}
