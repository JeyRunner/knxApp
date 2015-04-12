/* 
 * File:   TunnelingResponse.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * TUNNELINGRESPONSE    CLASS
 * tunneling response packet
 * -> send after get tunneling request
 * --------------------------------------
 */


#ifndef TUNNELINGRESPONSE_H
#define	TUNNELINGRESPONSE_H

#include "KnxPacket.h"
//#include "out.hpp"


class TunnelingResponse : public KnxPacket
{
    public:
        TunnelingResponse(KnxConnection* connection);
        TunnelingResponse(KnxConnection* connection, char bytes[]);
        
        // connection header
        char conHeaderLenght,
             conHeaderChanelID,
             conHeaderError;        
        
        // set variables by bytes
        virtual void fromBytes(char bytes[]);
        
        // get bytes
        virtual void toBytes(char bytes[]);
};

#endif	/* TUNNELINGRESPONSE_H */

