/* 
 * File:   TunnelingRequest.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * TUNNELINGREQUEST     CLASS
 * tunneling request packet
 * -> tunnel to knx bus
 * --------------------------------------
 */


#ifndef TUNNELINGREQUEST_H
#define	TUNNELINGREQUEST_H

#include "KnxPacket.h"
#include "KnxConnection.h"
#include "../../../protocol/src/KnxValue.h"
//#include "out.hpp"


class TunnelingRequest : public KnxPacket
{
    public:
        TunnelingRequest(KnxConnection *connection);
        TunnelingRequest(KnxConnection *connection, char bytes[]);
        
        // connection header
        char conHeaderLenght,
             *conHeaderChanelID,
             conHeaderCounter;
        
        // cEMI frame
        char cemiType,
             cemiInfoLenght,
             cemiControl,
             cemiDRL,
             cemiAddrSrcHi,
             cemiAddrSrcLo,
             cemiAddrDestHi,
             cemiAddrDestLo;
        
        // tunneling request sequence counter
        static int counter;

        // knx data value
        KnxValue *value;
        
        
        // set variables by bytes
        virtual void fromBytes(char bytes[]);
        
        // get bytes
        virtual void toBytes(char bytes[]);

        /* reset packet
         * use if want to send received packed again */
        void reset();
        
        // send packet
        virtual void send();
        
        // set knx source address
        virtual void setSourcAddr(int area, int groupe, int line);
        void getSourcAddr(int &area, int &groupe, int &line);
        
        // set knx destination address
        virtual void setDestinationAddr(int main, int middle, int sub);
        void getDestinationAddr(int &main, int &middle, int &sub);
};

#endif	/* TUNNELINGREQUEST_H */

