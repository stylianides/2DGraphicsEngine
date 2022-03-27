/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_engine_platform.h"
#include "home_engine_math.h"
#include "home_world.cpp"
#include "home_entity.h"

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



struct camera
{
    world_block Block;
    v3 P;
    
    v2 ScreenMapping;
    
    v2 RenderThickness;
    v4 RenderColour;
};

struct engine_state
{
    memory_arena PermanentArena;
    
    world World;
    
    // TODO(stylia): Put this on Render group
    real32 MetersToPixels;
    
    v4 BackDropColour;
    
#if DEBUG
    camera DebugCamera;
#endif
    camera GameCamera;
    
    entity Player;
    
    real32 tSin;
};


#endif //HOME_ENGINE_H
