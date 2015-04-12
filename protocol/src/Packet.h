/* 
 * File:   Packet.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Packet CLASS
 * - basic home protocol packet
 * - important fields
 * --> packet type
 * --------------------------------------
 */


#ifndef _KNX_PACKET_H_
#define _KNX_PACKET_H_

// include
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;
namespace home
{
class Client;

/* Packet class
 */
class Packet
{
    public:
        Packet();
        Packet(char bytes[]);
        ~Packet();

        // type
        char type,
             length;

        // set variables by bytes
        virtual void fromBytes(char bytes[]);

        // get bytes
        virtual void toBytes(char bytes[]);
};


// packet types
const char HOME_PACKET_KNXTUNNEL = 0x01,
           HOME_PACKET_DEVICE    = 0x02;


};     /* END NAMESPACE */
#endif /* _KNX_PACKET_H_ */
