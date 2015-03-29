/* 
 * File:   PacketFactory.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * PacketFactory CLASS
 * - creates packet objects
 * --> depending on packet type
 * - static use
 * --------------------------------------
 */

#include "PacketFactory.h"
using namespace home;


// -- CREATE PACKET ---------------------
Packet* PacketFactory::createPacket(char bytes[])
{
    // decide by packettype field
    switch (bytes[0])
    {
        case HOME_PACKET_KNXTUNNEL:
            break;

        case HOME_PACKET_DEVICE:
            break;

        default:
            cout << "[PFAC] unkown packettype '"<< bytes[0] <<"' [ERR]" << endl;
            return new Packet(bytes);
    }
}