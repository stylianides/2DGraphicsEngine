
#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

#include "home_engine.h"

#define MAX_CONTROLLERS 4
#define FILE_TIME_LATER 1

#define A_KEY 0x41 
#define D_KEY 0x44 
#define E_KEY 0x45 
#define F_KEY 0x46
#define P_KEY 0x50
#define S_KEY 0x53 
#define W_KEY 0x57 
#define SPACE_KEY 0x20
#define ESC_KEY 0x1B

struct win32_window_dim
{
    int32 Width;
    int32 Height;
};

struct win32_image_buffer
{
    BITMAPINFO BmpInfo;
    
    uint32 Width;
    uint32 Height;
    
    // TODO(stylia): think about this, do we need pitch?
    int32 Pitch; 
    
    void *Pixels;
};

struct win32_state
{
    void *Memory;
    
    char EngineDLLPath[256]; // NOTE(stylia): This is never loaded, only created
    FILETIME EngineDLL_LastWriteTime;
    
    HMODULE EngineDLL_Loaded;
    char EngineDLLPath_Loaded[256]; // NOTE(stylia): This is loaded
    
    engine_update_and_render *EngineUpdateAndRender;
    engine_output_sound *EngineOutputSound;
};
#endif //WIN32_PLATFORM_H