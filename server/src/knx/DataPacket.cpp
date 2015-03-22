/* 
 * File:   DataPacket.cpp
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * DATAPACKET     CLASSES
 * specific tunneling request packet's
 * --------------------------------------
 */


#include "DataPacket.h"


// ** SWITCH PACKET **********************************

SwitchPacket::SwitchPacket(KnxConnection* connection)
:TunnelingRequest(connection), SwitchData(){
}

SwitchPacket::SwitchPacket(KnxConnection* connection, char bytes[]) 
: TunnelingRequest(connection, bytes) 
{
    fromBytes(bytes);
}



void SwitchPacket::fromBytes(char bytes[]) 
{
    TunnelingRequest::fromBytes(bytes);
    
    // set data part
    data_ = bytes[20];
}

void SwitchPacket::toBytes(char bytes[])
{
    TunnelingRequest::toBytes(bytes);
    
    // set data part
    bytes[20] = data_;
}

bool SwitchPacket::switcH(bool switcH) 
{
    if (switcH)
        data_ = data_on;
    else 
        data_ = data_off;
    return switcH;
}

bool SwitchPacket::switcH() 
{
    if (data_ == data_on)
        return true;
    else 
        return false;
}




// ** DIM    PACKET **********************************

DimPacket::DimPacket(KnxConnection* connection) 
:TunnelingRequest(connection), DimData(){}

DimPacket::DimPacket(KnxConnection* connection, char bytes[])
:TunnelingRequest(connection, bytes) {}



void DimPacket::fromBytes(char bytes[]) 
{
    TunnelingRequest::fromBytes(bytes);
    
    // set data part
    data_ = bytes[20];
}

void DimPacket::toBytes(char bytes[]) 
{
    TunnelingRequest::toBytes(bytes);
    
    // set data part
    bytes[20] = data_;
}

float DimPacket::dim(float percent) 
{
    // @TODO dimmer convert
}

float DimPacket::dim() 
{
    // @TODO dimmer convert
}

