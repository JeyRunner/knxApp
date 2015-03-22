/* 
 * File:   DisconnectRespose.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * DISCONNECTRESPONSE     CLASS
 * answer to disconnect_request
 * --------------------------------------
 */


#ifndef DISCONNECTRESPOSE_H
#define	DISCONNECTRESPOSE_H

#include "KnxPacket.h"


class DisconnectRespose : public KnxPacket
{
    public:
        DisconnectRespose(KnxConnection *connection);
        DisconnectRespose(KnxConnection *connection, char bytes[]);
        
        // connection header
        char conHeaderLenght,
             *conHeaderChanelID,
             conHeaderError;
        
        
        // set variables by bytes
        virtual void fromBytes(char bytes[]);
        
        // get bytes
        virtual void toBytes(char bytes[]);
};

#endif	/* DISCONNECTRESPOSE_H */

