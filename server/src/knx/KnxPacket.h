/* 
 * File:   KnxPacket.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * KNXPACKET     CLASS
 * basic knxIp packet
 * --------------------------------------
 */


#ifndef KNXPACKET_H
#define	KNXPACKET_H

#include "KnxConnection.h"
//#include "out.hpp"


class KnxPacket 
{
    public:
        KnxPacket();
        KnxPacket(KnxConnection *connection);
        virtual ~KnxPacket();
        
        // knxConnection
        KnxConnection *connection;
        
        // error code
        char error;        
        
        // header
        char headerLenght,
             headerVersion,
             headerTypeHi,
             headerTypeLo,
             headerTotalLengthHi,
             headerTotalLengthLo;
        
        
        // set variables by bytes
        virtual void fromBytes(char bytes[]);
        
        // get bytes
        virtual void toBytes(char bytes[]);
        
        // send packet
        virtual void send();
             
        // get fitting packet
        static KnxPacket* getPacket(char bytes[], KnxConnection *connection);
        
        void  setType(short type);
        short getType();
        void  setTotalLength(short length);
        short getTotalLength();
};


// -- constant
// -- packet types
const short KNX_PACKET                      = 0x0000,
            KNX_PACKET_TUNNELING_REQUEST    = 0x0420,
            KNX_PACKET_TUNNELING_RESPONSE   = 0x0421,
            KNX_PACKET_DISCONNECT_REQUEST   = 0x0209,
            KNX_PACKET_DISCONNECT_RESPONSE  = 0x020A;



#endif	/* KNXPACKET_H */

