/* date = January 17th 2022 10:15 am */

#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

#include <cstdint>

#define global static
#define internal static
#define local_persist static

#if SLOW
#define Assert(Expr) if(!(Expr)){(int *)0 = 0;}
#else
#define Assert(Expr)
#endif
#define InvalidCodePath Assert(!"InvalidCodePath")


#define Pi 3.1415926535

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef size_t memory_index;


#define BYTES_PER_PIXEL 4


struct win32_window_dim
{
    int32 Width;
    int32 Height;
};


struct win32_image_buffer
{
    BITMAPINFO BmpInfo;
    
    int32 Width;
    int32 Height;
    int32 Pitch;
    
    void *Pixels;
};


#endif //WIN32_PLATFORM_H
