/* 
 * File:   DisconnectRequest.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * DisconnectRequest CLASS
 * connection partner should disconnect
 * --------------------------------------
 */


#ifndef _KNX_DISCONNECTREQUEST_H_
#define _KNX_DISCONNECTREQUEST_H_

// include
#include <iostream>
#include <string>
#include <stdlib.h>
#include "KnxPacket.h"

using namespace std;


/* DisconnectRequest class
 */
class DisconnectRequest : public KnxPacket
{
    public:
        DisconnectRequest(KnxConnection *connection);
        DisconnectRequest(KnxConnection *connection, char bytes[]);

        // connection header
        char *conHeaderChanelID;


        // set variables by bytes
        virtual void fromBytes(char bytes[]);

        // get bytes
        virtual void toBytes(char bytes[]);
};


#endif /* _KNX_DISCONNECTREQUEST_H_ */
