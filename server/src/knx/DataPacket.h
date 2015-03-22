/* 
 * File:   DataPacket.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * DATAPACKET     CLASSES
 * specific tunneling request packet's
 * --------------------------------------
 */


#ifndef DATAPACKET_H
#define	DATAPACKET_H

#include "TunnelingRequest.h"
#include "KnxData.h"
//#include "out.hpp"


/*
 * SwitchPacket class
 * specific tunneling request
 * switch ON / OFF
 */
class SwitchPacket : public TunnelingRequest, public SwitchData
{
    public:
        SwitchPacket(KnxConnection *connection);
        SwitchPacket(KnxConnection* connection, char bytes[]);

        
        virtual void fromBytes(char bytes[]);
        virtual void toBytes(char bytes[]);
        
        virtual bool switcH(bool switcH);
        virtual bool switcH();
        char data_;

    private:
        static const char 
             data_on  = 0x81,
             data_off = 0x80;
};



/*
 * DimPacket class
 * specific tunneling request
 * dimmer
 */
class DimPacket : public TunnelingRequest, public DimData
{
    public:
        DimPacket(KnxConnection* connection);
        DimPacket(KnxConnection* connection, char bytes[]);

        
        virtual void fromBytes(char bytes[]);
        virtual void toBytes(char bytes[]);
        
        virtual float dim(float percent);
        virtual float dim();


    private:
        char data_;
};


#endif	/* DATAPACKET_H */

