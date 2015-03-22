/* 
 * File:   TunnelingResponse.cpp
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * TUNNELINGRESPONSE    CLASS
 * tunneling response packet
 * -> send after get tunneling request
 * --------------------------------------
 */

#include "TunnelingResponse.h"

TunnelingResponse::TunnelingResponse(KnxConnection* connection): KnxPacket(connection)
{
    conHeaderChanelID = connection->chanelId;
    
    // default parameter 
    // when sending the packet
    setType(KNX_PACKET_TUNNELING_RESPONSE);
    setTotalLength(10);     // 10 byte
    conHeaderLenght = 0x04; // 4 byte
    conHeaderError    = 0x00;
    error             = conHeaderError;
}

TunnelingResponse::TunnelingResponse(KnxConnection* connection, char bytes[]): TunnelingResponse(connection)
{
    fromBytes(bytes);
}



// -- SET VAR FROM BYTES ----------------
void TunnelingResponse::fromBytes(char bytes[])
{
    // Header
    KnxPacket::fromBytes(bytes);
    
    // Connection Header (4 Bytes)
    conHeaderLenght     = bytes[6];
    conHeaderChanelID   = bytes[7];
    conHeaderError      = bytes[9];
    
    // set error;
    error = conHeaderError;
}


// -- GET BYTES FROM VAR ----------------
void TunnelingResponse::toBytes(char bytes[]) 
{
    // Header
    KnxPacket::toBytes(bytes);
    
    // Connection Header (4 Bytes)
    bytes[6] = conHeaderLenght;
    bytes[7] = conHeaderChanelID & 0xff;
    bytes[8] = 0x01; // 01 data byte following
    bytes[9] = conHeaderError;
}