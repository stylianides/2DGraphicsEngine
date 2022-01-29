/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_platform.h"

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

internal void
FreeArena(mem_arena *Arena)
{
    if(Arena->Base)
    {
        VirtualFree(Arena->Base, 0, MEM_RELEASE);
    }
    Arena->Size = 0;
    Arena->Used = 0;
    Arena->Base = 0;
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

enum engine_button_types
{
    Input_Up = 0,
    Input_Down = 1,
    Input_Left = 2,
    Input_Right = 3,
    Input_Jump = 4,
    Input_Attack = 5,
    Input_Power = 6,
    Input_Start  = 7,
};

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
    
    engine_input_button Buttons[8];
};

// TODO(stylia): Is it better if keyboard is on the ControllersArray
struct engine_input
{
    engine_input_controller Keyboard;
    engine_input_controller Controllers[MAX_PLAYERS];
};

struct engine_state
{
    mem_arena PermanentArena;
    mem_arena TransientArena;
    
    engine_input Input;
};

#endif //HOME_ENGINE_H
