
#ifndef HOME_INTRINSICS_H
#define HOME_INTRINSICS_H

#include "math.h"

int32 RoundRealToInt32(real32 Value)
{
#ifdef MSVC
    int32 Result = (int32)roundf(Value);
#elif GCC
    int32 Result = (int32)roundf(Value);
#else
    int32 Result = (int32)roundf(Value);
#endif
    
    return(Result);
}

uint32 RoundRealToUInt32(real32 Value)
{
#ifdef MSVC
    uint32 Result = (uint32)roundf(Value);
#elif GCC
    uint32 Result = (uint32)roundf(Value);
#else
    uint32 Result = (uint32)roundf(Value);
#endif
    
    return(Result);
}

#endif 
