
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
    
    engine_state *State = (engine_state *)Memory;
    memory_arena *PermArena = &State->PermanentArena;
    world *World = &State->World;
    
    ClearScreen(Buffer, State->BackDropColour);
    
    // NOTE(stylia): Assume that the first bytes of memory are the engine state
    
    
    if(!Memory->IsMemoryInitialized)
    {
        InitializeArena(PermArena,
                        Memory->PermanentMemorySize - sizeof(engine_state),
                        (uint8 *)Memory + sizeof(engine_state));
        
        State->MetersToPixels = 100.0f;
        State->BackDropColour = V4(0.3f, 0.3f, 0.3f, 0.3f);
        
        State->Player.Thickness = V2(30.0f, 30.0f);
        State->Player.Colour = V4(0.5f, 0.0f, 0.0f, 1.0f);
        
        State->World.BlockDimMeters = BLOCK_DIM_METERS;
        
        int32 BlockX = INT_MAX / 2;
        int32 BlockY = INT_MAX / 2;
        
        world_position InitialPosition = {BlockX, BlockY, 0, V3(0.0f, 0.0f, 0.0f)};
        
        State->GameCamera.Pos = InitialPosition;
        State->DebugCamera.Pos = InitialPosition;
        State->DebugCamera.RenderThickness = V2(5.0f, 5.0f);
        State->Player.Pos = InitialPosition;
        State->Player.Pos.Offset_ = V3(1.3f, 0.5f, 0.0f);
        
        for(uint32 Block = 0; Block < 4096; ++Block)
        {
            if(Block % 2 == 0)
            {
                World->Blocks[Block].X = BlockX++;
            }
            else
            {
                World->Blocks[Block].Y = BlockY++;
            }
        }
        
        Memory->IsMemoryInitialized = true;
    }
    
    // NOTE(stylia): Render Camera Point
    v2 CameraThickness = {5, 5};
    v2 CameraPos = V2((real32)(Buffer->Width / 2), (real32)(Buffer->Height / 2));
    v4 CameraColour = V4(0.5f, 0.1f, 0.3f, 1.0f);
    
    v2 CameraCenter = V2((real32)(Buffer->Width/2), (real32)(Buffer->Height/2));
    v2 CameraTopLeft = CameraCenter - State->DebugCamera.RenderThickness;
    v2 CameraBottomRight = CameraCenter + State->DebugCamera.RenderThickness;
    DrawRectangle(Buffer, CameraTopLeft, CameraBottomRight, State->DebugCamera.RenderColour);
    
    
    // TODO(stylia): make player controller mapping
    for(uint32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ControllerIndex++)
    {
        engine_input_controller *Controller = Input->Controllers + ControllerIndex;
        
        if(Controller->IsConnected)
        {
            v3 PlayerCameraDistance =
                Difference(World, State->DebugCamera.Pos, State->Player.Pos);
            
            v2 PlayerCenter = CameraPos - State->MetersToPixels * PlayerCameraDistance.xy;
            v2 PlayerTopLeft = PlayerCenter - State->Player.Thickness;
            v2 PlayerBottomRight = PlayerCenter + State->Player.Thickness;
            
            DrawRectangle(Buffer, PlayerTopLeft, PlayerBottomRight, State->Player.Colour);
        }
    }
    
    
    
    
    
    END_TIME_BLOCK(Sections_UpdateAndRender);
}

