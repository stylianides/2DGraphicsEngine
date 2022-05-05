#ifndef HOME_PLATFORM_H
#define HOME_PLATFORM_H

#if __GNUG__
#define GCC 
#elif _MSC_VER
#define MSVC
#endif

#define global static
#define internal static
#define local_persist static

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))

#if SLOW_BUILD
#define Assert(Expr) if(!(Expr)){*(int *)0 = 0;}
#else
#define Assert(Expr)
#endif

#define InvalidCodePath Assert(!"InvalidCodePath")

#define INT_MAX 2147483647

#define Pi32 3.1415926535f

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

typedef int8 i8;
typedef int16 i16;
typedef int32 i32;
typedef int64 i64;

typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef int32 bool32;
typedef bool32 b32;

typedef float real32;
typedef double real64;

typedef real32 r32;
typedef real64 r64;

typedef size_t memory_index;

enum CodeSections
{
    Sections_UpdateAndRender,
    Sections_Size,
};

#if DEBUG
#include <intrin.h>

struct ClockTimer
{
    uint64 Cycles;
    uint32 HitCount;
};

#ifdef MSVC
#define BEGIN_TIME_BLOCK(ID) uint64 Start##ID = __rdtsc();
#define END_TIME_BLOCK(ID) DebugMemory->Timers[ID].Cycles += __rdtsc() - Start##ID; ++DebugMemory->Timers[ID].HitCount;
#endif

#else

#define BEGIN_TIME_BLOCK
#define END_TIME_BLOCK

#endif


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

struct engine_input
{
    real32 dt;
    
    // NOTE(stylia): Keyboard is always Controllers[0]
    engine_input_controller Controllers[MAX_PLAYERS + 1];
};

struct engine_sound
{
    int32 Hz;
    
    // NOTE(stylia): Both Right and Left 
    int16 SampleSize;
    
    int32 SamplesPerSecond;
    int32 SampleCount;
    
    uint32 SampleBufferSize;
    int16 *SampleBuffer;
    
};

struct engine_image
{
    void *Pixels;
    
    int32 Width;
    int32 Height;
    
    uint32 Pitch;
};

struct engine_memory
{
    void *Memory;
    bool32 IsMemoryInitialized;
    
    memory_index PermanentMemorySize;
    memory_index TransientMemorySize;
    
    debug_platform_read_file *DEBUGPlatformReadFile;
    debug_platform_write_file *DEBUGPlatformWriteFile;
    debug_platform_free_file *DEBUGPlatformFreeFile;
    
#if DEBUG
    ClockTimer Timers[Sections_Size];
#endif
};

#define ENGINE_UPDATE_AND_RENDER(Name) void Name(engine_memory *Memory, engine_input *Input, engine_image *Buffer)
typedef ENGINE_UPDATE_AND_RENDER(engine_update_and_render);
extern "C" ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender);


#define ENGINE_OUTPUT_SOUND(Name) void Name(engine_memory *Memory, engine_sound *Sound)
typedef ENGINE_OUTPUT_SOUND(engine_output_sound);
extern "C" ENGINE_OUTPUT_SOUND(EngineOutputSound);
#endif //HOME_PLATFORM_H
