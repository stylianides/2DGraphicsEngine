
#include "home_engine.h"
#include "home_render_group.cpp"

extern "C"
ENGINE_OUTPUT_SOUND(EngineOutputSound)
{
#if 0
    int16 ToneVolume = 4000;
    
    real32 Period = 1.0f / (real32)(460);
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

#if DEBUG
engine_memory *DebugMemory;
#endif

extern "C"
ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender)
{
#if DEBUG
    DebugMemory = Memory;
#endif
    
    BEGIN_TIME_BLOCK(Sections_UpdateAndRender);
    
    // NOTE(stylia): Assume that the first bytes of memory are the engine state
    engine_state *State = (engine_state *)Memory;
    
    if(!Memory->IsMemoryInitialized)
    {
        InitializeArena(&State->PermanentArena, 
                        Memory->PermanentMemorySize - sizeof(engine_state),
                        (uint8 *)Memory + sizeof(engine_state));
        
        State->Player.P = V2(40.0f, 40.0f);
        State->Player.PDim = V2(30.0f, 30.0f);
        State->BackDrop = V4(1.0f, 0.5f, 0.3f, 0.1f);
        
        State->World.TileEdgeInMeters = 1.0f;
        
        Memory->IsMemoryInitialized = true;
    }
    
    ClearScreen(Buffer, State->BackDrop);
    
    
    // TODO(stylia): make player controller mapping
    for(uint32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ControllerIndex++)
    {
        engine_input_controller *Controller = Input->Controllers + ControllerIndex;
        
        if(Controller->IsConnected)
        {
            DrawRectangle(Buffer, State->Player.P, State->Player.P + State->Player.PDim, V4(0.5f, 0.5f, 0.5f, 0.5));
        }
    }
    
    for(int32 X = 0; X < 1000; ++X)
    {
        
    }
    
    
    END_TIME_BLOCK(Sections_UpdateAndRender);
}

