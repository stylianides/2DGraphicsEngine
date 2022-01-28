/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_platform.h"

#define MAX_PLAYERS 2

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
    bool32 IsConnected;
    engine_input_button Buttons[8];
};

struct engine_input
{
    
    engine_input_controller Controllers[MAX_PLAYERS];
};

struct engine_state
{
    mem_arena PermanentArena;
    mem_arena TransientArena;
    
    engine_input Input;
};

#endif //HOME_ENGINE_H
