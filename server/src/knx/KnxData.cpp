/* 
 * File:   KnxData.cpp
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


#include "KnxData.h"

/* ** SWITCH DATA ******************************************** */
bool SwitchData::switcH(bool switcH) {
    this->value = switcH;
    return value;
}

bool SwitchData::switcH() {
    return value;
}

void SwitchData::set(KnxData* data) {
    if ((SwitchData*)(data))
        this->switcH(((SwitchData*)data)->switcH());
}


/* ** DIM    DATA ******************************************** */
float DimData::dim(float percent) {
    this->value = percent;
    return value;
}

float DimData::dim() {
    return value;
}

void DimData::set(KnxData* data) {
    if ((DimData*)(data))
        this->dim(((DimData*)data)->dim());
}

