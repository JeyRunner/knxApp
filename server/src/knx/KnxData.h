/* 
 * File:   KnxData.h
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * KNXDATA     CLASSES
 * store knx data
 * types:
 *  - switch
 *  - dimm
 * --------------------------------------
 */


#include "TunnelingRequest.h"
//#include "out.hpp"

#ifndef KNXDATA_H
#define	KNXDATA_H


/*
 * KnxData class
 * store knx data 
 */
class KnxData
{
    public:
        // copy values from other object
        virtual void set(KnxData *data) = 0;
        
};



/*
 * SwitchData class
 * store knx switch data
 * ON / OFF
 */
class SwitchData : public KnxData
{
    public:
        // copy values from other object
        virtual void set(KnxData *data);
        
        /* switch 
         * @param true  - ON
         *    false - OFF 
         */
        virtual bool switcH(bool switcH);
        virtual bool switcH();
    
        
    private:
        bool value;
};



/*
 * DimData class
 * store knx dimmer data
 */
class DimData : public KnxData
{
    public:
        // copy values from other object
        virtual void set(KnxData *data);
        
        /* dim
         * @param min = 0
         *    max = 100
         */
        virtual float dim(float percent);
        virtual float dim();
            
        
    private:
        float value;
};



#endif	/* KNXDATA_H */

