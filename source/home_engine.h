/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_engine_platform.h"

#define MAX_PLAYERS 2

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

struct engine_input_button
{
    uint32 Press;
};

struct engine_input_controller
{
    bool32 IsAnalog;
    bool32 IsConnected;
    
    // NOTE(stylia): Normalized
    real32 StickX;
    real32 StickY;
    
    union
    {
        struct
        {
            engine_input_button Up;
            engine_input_button Down;
            engine_input_button Left;
            engine_input_button Right;
            engine_input_button Jump;
            engine_input_button Attack;
            engine_input_button Power;
            engine_input_button Start;
        };
        engine_input_button ButtonList[8];
    }Buttons;
};

// TODO(stylia): Is it better if keyboard is on the ControllersArray?
struct engine_input
{
    engine_input_controller Keyboard;
    engine_input_controller Controllers[MAX_PLAYERS];
};

struct engine_sound
{
    
};

struct engine_image
{
    
};

struct engine_state
{
    void *Memory;
    mem_index MemorySize;
    bool32 IsMemoryInitialized;
    
    mem_arena PermanentArena;
    mem_index PermanentArenaSize;
    
    mem_arena TransientArena;
    mem_index TransientArenaSize;
};

extern "C" void EngineUpdateAndRender(engine_state *State, engine_input *Input, engine_image *Buffer);

extern "C" void EngineOutputSound(engine_state *State, engine_sound *Sound);


#endif //HOME_ENGINE_H
