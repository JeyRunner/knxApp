/* 
 * File:   KnxTunnelPacket.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * KnxTunnelPacket CLASS
 * communicate with knx bus
 * --------------------------------------
 */

#include "KnxPacket.h"
using namespace home;


// -- CREATE OBJECT ---------------------
KnxPacket::KnxPacket() : Packet()
{
    // set fields
    type    = HOME_PACKET_KNXTUNNEL;
    length  = 7;                        // data 2 bytes long

    // create default switch value
    value = new SwitchValue();
}

KnxPacket::KnxPacket(char bytes[]) : KnxPacket::KnxPacket()
{
    fromBytes(bytes);
}


// -- SET VAR FROM BYTES ----------------
void KnxPacket::fromBytes(char bytes[])
{
    Packet::fromBytes(bytes);

    addrSrcHi   = bytes[1];
    addrSrcLo   = bytes[2];
    addrDestHi  = bytes[3];
    addrDestLo  = bytes[4];

    // data
    value->fromBytes(bytes, OFFSET_DATA);
}


// -- GET BYTES FROM VAR ----------------
void KnxPacket::toBytes(char bytes[])
{
    Packet::toBytes(bytes);

    bytes[1] = addrSrcHi   & 0xff;
    bytes[2] = addrSrcLo   & 0xff;
    bytes[3] = addrDestHi  & 0xff;
    bytes[4] = addrDestLo  & 0xff;

    // data
    value->toBytes(bytes, OFFSET_DATA);
}


// -- SET GET ---------------------------
void KnxPacket::setSourcAddr(int area, int groupe, int line)
{
    addrSrcHi = (groupe & 0b00000111) | ((area << 3));
    addrSrcLo = line;
}

void KnxPacket::getSourcAddr(int &area, int &groupe, int &line)
{
    line   = (int)addrSrcLo;
    groupe = (int)(addrSrcHi & 0b00000111);
    area   = (int)(addrSrcHi >> 3);
}


void KnxPacket::setDestinationAddr(int main, int middle, int sub)
{
    addrDestHi = (middle & 0b00000111) | ((main << 3));
    addrDestLo = sub;
}

void KnxPacket::getDestinationAddr(int &main, int &middle, int &sub)
{

    sub    = (int)addrDestLo;
    middle = (int)(addrDestHi & 0b00000111);
    main   = (int)(addrDestHi >> 3);
}


// -- DESTROY OBJECT --------------------
KnxPacket::~KnxPacket()
{
}