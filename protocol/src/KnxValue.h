/* 
 * File:   KnxValue.h
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


#ifndef KNX_KNXVALUE_H
#define KNX_KNXVALUE_H

// include
#include <iostream>
#include <string>
#include <stdlib.h> 

using namespace std;


/* KnxValue class
 */
class KnxValue
{
    public:
        // value type (KNX_VALUE_...)
        char type;

        // set variables by bytes (on offset position)
        virtual void fromBytes(char bytes[], int offset) = 0;

        // create bytes (on offset position)
        virtual void toBytes(char bytes[], int offset) = 0;
};


/* SwtichValue class
 */
class SwitchValue : public KnxValue
{
    public:
        SwitchValue();

        /* set
         * on  - true
         * off - false */
        bool set(bool value);
        bool get();

        // set variables by bytes (on offset position)
        virtual void fromBytes(char bytes[], int offset);

        // create bytes (on offset position)
        virtual void toBytes(char bytes[], int offset);

    private:
        bool value;
        static const char ON_BYTE  = 0x81,
                          OFF_BYTE = 0x80;
};


/* SwtichValue class
 */
class DimValue : public KnxValue
{
    public:
        DimValue();

        /* set
         * range 0 - 255 */
        int set(int value);
        int get();

        // set variables by bytes (on offset position)
        virtual void fromBytes(char bytes[], int offset);

        // create bytes (on offset position)
        virtual void toBytes(char bytes[], int offset);

    private:
        char value;
};


// const
const char KNX_VALUE_SWITCH = 1,
           KNX_VALUE_DIM    = 2;


#endif /* KNX_KNXVALUE_H */
