
#include "home_engine.h"
#include "home_render_group.cpp"

extern "C"
ENGINE_OUTPUT_SOUND(EngineOutputSound)
{
    real32 SecondsPerFlip = 1.0f / Sound->Hz;
    int32 SamplesPerFlip = (int32)(Sound->SamplesPerSecond * SecondsPerFlip);
    
    uint16 Volume = 130;
    
    uint32 BytesWritten = 0;
    uint16 *Current = (uint16 *)Sound->Samples;
    
    /*while(BytesWritten < Sound->SampleBufferSize)
    {
        *Current++ = (int16)(sinf(State->tSin)*Volume); // NOTE(stylia): first channel
        *Current++ = (int16)(sinf(State->tSin)*Volume); // NOTE(stylia): second channel
        
        BytesWritten += 4;
        
        State->tSin += 2.0f*Pi32*(1.0f/(real32)Sound->Hz);
        if(State->tSin > 2.0f*Pi32)
        {
            State->tSin -= 2.0f*Pi32;
        }
    }*/
}

extern "C"
ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender)
{
    if(!State->IsMemoryInitialized)
    {
        State->P = V2(40.0f, 40.0f);
        
        State->IsMemoryInitialized = true;
    }
    
    // NOTE(stylia): This is bad code for testing save state and input loops
    
    
    
    
    int32 PlayerRadiusX = 20;
    int32 PlayerRadiusY = 20;
    
    // NOTE(stylia): Try the HotLoading by changing Colours!
    ClearScreen(Buffer, V4(0.0f, 0.0f, 1.0f, 0));
    DrawRectangleOutline(Buffer, V2(5.0f, 5.0f), V2(500.0f, 500.0f), 2,
                         V4(0.0f, 0.0f, 0.0f, 0));
    
    
    int a = 1;
    
    int testcode = 0;
}

