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
#include "Log.h"
using namespace home;
home::Log PacketFactory::log;


// -- CREATE PACKET ---------------------
Packet* PacketFactory::createPacket(char bytes[])
{
    log.setLogName("PFAC");

    // decide by packettype field
    switch (bytes[0])
    {
        case HOME_PACKET_KNXTUNNEL:
            return new KnxPacket(bytes);
            break;

        case HOME_PACKET_DEVICE:
            break;

        default:
            log.err("unkown packettype '"+ to_string(bytes[0]) +"'");
            return new Packet(bytes);
    }
}