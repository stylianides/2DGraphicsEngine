/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_engine_platform.h"

struct memory_arena
{
    memory_index Size;
    memory_index Used;
    uint8 *Base;
};

internal void
InitializeArena(memory_arena *Arena, memory_index Size, uint8 *Base)
{
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->Base = Base;
    Assert(Arena->Base);
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, type, Amount) (type *)PushSize_(Arena, Amount*sizeof(type))

internal void *PushSize_(memory_arena *Arena, memory_index Size)
{
    Assert(Arena->Used + Size <= Arena->Size);
    
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return(Result);
}

struct entity
{
    v2 P;
    v2 dP;
    v2 ddP;
    
    v2 PDim;
};

struct engine_state
{
    memory_arena PermanentArena;
    
    world World;
    
    entity Player;
    v4 BackDrop;
    
    real32 tSin;
};


#endif //HOME_ENGINE_H
