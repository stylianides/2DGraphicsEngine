
#include "home_engine.h"
#include "home_render_group.cpp"

extern "C"
ENGINE_OUTPUT_SOUND(EngineOutputSound)
{
#if 1
    int16 ToneVolume = 4000;
    
    real32 Period = 1.0f / (real32)(400);
    real32 SamplesOverPeriod = Period * (real32)Sound->SamplesPerSecond;
    real32 Interval = (2.0f*Pi32) / SamplesOverPeriod;
    
    int16 *Dest = (int16 *)Sound->SampleBuffer;
    
    for(int32 SampleIndex = 0;
        SampleIndex < Sound->SampleCount;
        SampleIndex++)
    {
        real32 SinValue = sinf(State->tSin);
        int16 SampleValue = (int16)(SinValue * ToneVolume);
        *Dest++ = SampleValue;
        *Dest++ = SampleValue;
        
        State->tSin += Interval;
        if(State->tSin > (2.0f * Pi32))
        {
            State->tSin = -2.0f*Pi32;
        }
    }
#endif
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

