
#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

#include "home_engine.h"

#define MAX_CONTROLLERS 4
#define FILE_TIME_LATER 1

#define A_KEY 0x41
#define B_KEY 0x42
#define C_KEY 0x43
#define D_KEY 0x44
#define E_KEY 0x45
#define F_KEY 0x46
#define G_KEY 0x47
#define H_KEY 0x48
#define I_KEY 0x49
#define J_KEY 0x4A
#define K_KEY 0x4B
#define L_KEY 0x4C
#define M_KEY 0x4D
#define N_KEY 0x4E
#define O_KEY 0x4F
#define P_KEY 0x50
#define Q_KEY 0x51
#define R_KEY 0x52
#define S_KEY 0x53
#define T_KEY 0x54
#define U_KEY 0x55 
#define V_KEY 0x56 
#define W_KEY 0x57 
#define X_KEY 0x58 
#define Y_KEY 0x59 
#define Z_KEY 0x5A 

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

struct win32_engine_code
{
    char EngineDLLPath[MAX_PATH]; // NOTE(stylia): This is never loaded, only created
    FILETIME EngineDLL_LastWriteTime;
    
    HMODULE EngineDLL_Loaded;
    char EngineDLLPath_Loaded[MAX_PATH]; // NOTE(stylia): This is loaded
    
    engine_update_and_render *EngineUpdateAndRender;
    engine_output_sound *EngineOutputSound;
};

struct win32_replay_stream
{
    HANDLE RecordFile;
    
    char Filename[MAX_PATH];
    
    bool32 IsRecording;
    bool32 IsPlayingBack;
    
    uint32 PlayingIndex;
    uint32 RecordingIndex;
};

struct win32_state
{
    void *Memory;
    
    mem_index PermanentMemorySize;
    mem_index TransientMemorySize;
    
    win32_replay_stream ReplayStream;
    
    
};
#endif //WIN32_PLATFORM_H