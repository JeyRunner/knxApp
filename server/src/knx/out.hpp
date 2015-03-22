/* 
 * File:   out.cpp
 * Author: joshua
 * 
 * Created on 27. Februar 2015, 17:16
 */

//#include "out.h"

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}