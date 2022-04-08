
#ifndef HOME_INTRINSICS_H
#define HOME_INTRINSICS_H

#include "math.h"

int32 RoundReal32ToInt32(real32 Value)
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


uint8 
BitScanForward32(uint32 Mask)
{
#ifdef MSVC
    unsigned long Result = 0;
    _BitScanForward(&Result, Mask);
#elif GCC
    // TODO(stylia): find gcc equivalent
#else
    // TODO(stylia): implement this myself
#endif
    
    return((uint8)Result);
}

// TODO(stylia): maybe intrinsic this?
inline int32 
Absolutei32(int32 Value)
{
    int32 Result = (Value < 0) ? (-Value) : (Value);
    
    return(Result);
}

inline real32 
Absolute(real32 Value)
{
    real32 Result = (Value < 0) ? (-Value) : (Value);
    
    return(Result);
}

// TODO(stylia): Intinsic this
real32 Sqrt(real32 Value)
{
    real32 Result = sqrtf(Value);
    
    return(Result);
}

#endif 
