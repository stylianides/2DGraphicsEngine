/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_engine_platform.h"



struct mem_arena
{
    mem_index Size;
    mem_index Used;
    uint8 *Base;
};

internal void
InitializeArena(mem_arena *Arena, mem_index Size)
{
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->Base = 0;
    Assert(Arena->Base);
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, type, Amount) (type *)PushSize_(Arena, Amount*sizeof(type))

internal void *PushSize_(mem_arena *Arena, mem_index Size)
{
    Assert(Arena->Used + Size <= Arena->Size);
    
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return(Result);
}

#endif //HOME_ENGINE_H
