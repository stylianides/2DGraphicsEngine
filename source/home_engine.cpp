
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

extern "C"
ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender)
{
    if(!State->IsMemoryInitialized)
    {
        State->P = V2(40.0f, 40.0f);
        State->PDim = V2(30.0f, 30.0f);
        State->IsMemoryInitialized = true;
        State->BackDrop = V4(1.0f, 0.5f, 0.3f, 0.1f);
    }
    
    // NOTE(stylia): This is bad code for testing save state and input loops
    
    // TODO(stylia): this is terrible, remove this
    // TODO(stylia): make player controller mapping
    for(uint32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ControllerIndex++)
    {
        engine_input_controller *Controller = Input->Controllers + ControllerIndex;
        
        int32 MovingDistancePixels = 10;
        
        if(Controller->Buttons.Up.Press)
        {
            State->P += V2i(0, +MovingDistancePixels);
        }
        
        if(Controller->Buttons.Down.Press)
        {
            State->P += V2i(0, -MovingDistancePixels);
        }
        
        if(Controller->Buttons.Right.Press)
        {
            State->P += V2i(MovingDistancePixels, 0);
        }
        
        if(Controller->Buttons.Left.Press)
        {
            State->P += V2i(-MovingDistancePixels, 0);
        }
    }
    
    
    // NOTE(stylia): Try the HotLoading by changing Colours!
    State->BackDrop += V4(0.003f, 0.003f, 0.003f, 0.001f);
    ClearScreen(Buffer, State->BackDrop);
    /*DrawRectangleOutline(Buffer, V2(5.0f, 5.0f), V2(500.0f, 500.0f), 2,
                         V4(0.0f, 0.0f, 0.0f, 0));*/
    
    
    DrawRectangle(Buffer, State->P, State->P + State->PDim, V4(0.5f, 0.5f, 0.5f, 0.5));
    
    
    int testcode = 0;
}

