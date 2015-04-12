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


#ifndef _KNX_PACKETFACTORY_H_
#define _KNX_PACKETFACTORY_H_

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include "Packet.h"
#include "KnxPacket.h"


using namespace std;
namespace home
{
class Client;

/* PacketFactory class
 */
class PacketFactory
{
    public:
        // create packet form bytes
        static home::Packet *createPacket(char bytes[]);
};


};     /* END NAMESPACE */
#endif /* _KNX_PACKETFACTORY_H_ */
