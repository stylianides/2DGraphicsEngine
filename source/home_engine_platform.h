#ifndef HOME_PLATFORM_H
#define HOME_PLATFORM_H

#define global static
#define internal static
#define local_persist static

#if SLOW_BUILD
#define Assert(Expr) if(!(Expr)){*(int *)0 = 0;}
#else
#define Assert(Expr)
#endif
#define InvalidCodePath Assert(!"InvalidCodePath")

#define Pi 3.1415926535

#define BYTES_PER_PIXEL 4

#define MAX_PLAYERS 2

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

struct file_contents
{
    void *Contents;
    uint32 Size;
};

#define DEBUG_PLATFORM_READ_FILE(name) file_contents name(char *Filename)
typedef DEBUG_PLATFORM_READ_FILE(debug_platform_read_file);

#define DEBUG_PLATFORM_WRITE_FILE(name) void name(char *Filename, void *BytesToWrite, uint32 Size)
typedef DEBUG_PLATFORM_WRITE_FILE(debug_platform_write_file);

#define DEBUG_PLATFORM_FREE_FILE(name) void name(file_contents FileContents)
typedef DEBUG_PLATFORM_FREE_FILE(debug_platform_free_file);

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

// TODO(stylia): Place keyboard is on the ControllersArray
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
    void *Pixels;
    
    uint32 Width;
    uint32 Height;
    uint32 Pitch;
};

#include "home_engine_math.h"

struct engine_state
{
    void *Memory;
    mem_index MemorySize;
    bool32 IsMemoryInitialized;
    
    v2 P;
    
    debug_platform_read_file *DEBUGPlatformReadFile;
    debug_platform_write_file *DEBUGPlatformWriteFile;
    debug_platform_free_file *DEBUGPlatformFreeFile;
};


#define ENGINE_UPDATE_AND_RENDER(Name) void Name(engine_state *State, engine_input *Input, engine_image *Buffer)
typedef ENGINE_UPDATE_AND_RENDER(engine_update_and_render);
extern "C" ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender);


#define ENGINE_OUTPUT_SOUND(Name) void Name(engine_state *State, engine_sound *Sound)
typedef ENGINE_OUTPUT_SOUND(engine_output_sound);
extern "C" ENGINE_OUTPUT_SOUND(EngineOutputSound);
#endif //HOME_PLATFORM_H
