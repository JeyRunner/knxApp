/* 
 * File:   KnxPacket.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * KnxPacket CLASS
 * communicate with knx bus
 * --------------------------------------
 */


#ifndef _KNX_KNXPACKET_H_
#define _KNX_KNXPACKET_H_

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include "Packet.h"
#include "KnxValue.h"

using namespace std;
namespace home
{

/* KnxPacket class
 */
class KnxPacket : public home::Packet
{
    public:
        KnxPacket();
        KnxPacket(char bytes[]);
        ~KnxPacket();

        // fields
        char datatype,
             addrSrcHi,
             addrSrcLo,
             addrDestHi,
             addrDestLo;

        // knx data value
        KnxValue *value;

        // set variables by bytes
        virtual void fromBytes(char bytes[]);

        // get bytes
        virtual void toBytes(char bytes[]);

        // set knx source address
        virtual void setSourcAddr(int area, int groupe, int line);
        void getSourcAddr(int &area, int &groupe, int &line);

        // set knx destination address
        virtual void setDestinationAddr(int main, int middle, int sub);
        void getDestinationAddr(int &main, int &middle, int &sub);

    private:
        // start of knx data bytes
        static const int OFFSET_DATA = 5;
};



};     /* END NAMESPACE */
#endif /* _KNX_KNXPACKET_H_ */
