/* 
 * File:   Packet.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Packet CLASS
 * @...
 * --------------------------------------
 */

#include "Packet.h"

using namespace home;


// -- CREATE OBJECT ---------------------
Packet::Packet()
{
    // default parameter
    // when sending the packet
    type = 0x00;
}

Packet::Packet(char bytes[]) : Packet()
{
    fromBytes(bytes);
}


// -- SET VAR FROM BYTES ----------------
void Packet::fromBytes(char bytes[])
{
    type = bytes[0];
}


// -- GET BYTES FROM VAR ----------------
void Packet::toBytes(char bytes[])
{
    bytes[0] = type;
}


// -- DESTROY OBJECT --------------------
Packet::~Packet()
{
}