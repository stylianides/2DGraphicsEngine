#ifndef HOME_PLATFORM_H
#define HOME_PLATFORM_H

#define global static
#define internal static
#define local_persist static

#if SLOW_BUILD
#define Assert(Expr) if(!(Expr)){(int *)0 = 0;}
#else
#define Assert(Expr)
#endif
#define InvalidCodePath Assert(!"InvalidCodePath")

#define Pi 3.1415926535

#define BYTES_PER_PIXEL 4

#define KiloBytes(Amount) 1024*(Amount)
#define MegaBytes(Amount) 1024*KiloBytes((Amount))
#define GigaBytes(Amount) 1024*MegaBytes((Amount))
#define TeraBytes(Amount) 1024*GigaBytes((Amount))

#define ArrayLength(Array) ((sizeof(Array))/(sizeof(Array[0])))

#define Min(A, B) (((A) < (B)) ? (A) : (B))
#define Max(A, B) (((A) > (B)) ? (A) : (B))

#include <cstdint>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int32 bool32;

typedef float real32;
typedef double real64;

typedef size_t mem_index;

#endif //HOME_PLATFORM_H
