/* 
 * File:   KnxValue.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * KnxValue CLASS
 * store knx values(data part in TUNNELING PACKETS):
 * - switch (bool)
 * - dim    (int)
 * - text   (string)
 * - ...
 * --------------------------------------
 */

#include "KnxValue.h"


// ** SWITCH *********************************************
// -- CREATE OBJECT ---------------------
SwitchValue::SwitchValue()
{
    // var
    type = KNX_VALUE_SWITCH;
}

// -- SET GET ---------------------------
bool SwitchValue::set(bool value)
{ this->value = value; return this->value; }

bool SwitchValue::get()
{ return this->value; }

// -- FROM TO BYTES ----------------------
void SwitchValue::fromBytes(char bytes[], int offset)
{
    switch (bytes[offset])
    {
        case ON_BYTE:
            value = true;
            break;

        case OFF_BYTE:
            value = false;
            break;
    }
}

void SwitchValue::toBytes(char bytes[], int offset)
{
    if (value)
    {
        bytes[offset] = ON_BYTE;
    }
    else
    {
        bytes[offset] = OFF_BYTE;
    }
}



// ** DIM *************************************************
// -- CREATE OBJECT ---------------------
DimValue::DimValue()
{
    // var
    type = KNX_VALUE_DIM;
}

// -- SET GET ---------------------------
int DimValue::set(int value)
{ this->value = value; return this->value; }

int DimValue::get()
{ return this->value; }

// -- FROM TO BYTES ----------------------
void DimValue::fromBytes(char bytes[], int offset)
{
    // @TODO DimValue fromBytes
}

void DimValue::toBytes(char bytes[], int offset)
{
    // @TODO DimValue toBytes
}