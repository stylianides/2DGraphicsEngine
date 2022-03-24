
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
    
    ClearScreen(Buffer, State->BackDrop);
    
    // NOTE(stylia): Assume that the first bytes of memory are the engine state
    
    
    if(!Memory->IsMemoryInitialized)
    {
        InitializeArena(PermArena,
                        Memory->PermanentMemorySize - sizeof(engine_state),
                        (uint8 *)Memory + sizeof(engine_state));
        
        State->Player.P = V2(40.0f, 40.0f);
        State->Player.PDim = V2(30.0f, 30.0f);
        State->BackDrop = V4(0.3f, 0.3f, 0.3f, 0.3f);
        
        State->World.TileDim = 2.0f;
        
        Memory->IsMemoryInitialized = true;
        
        World->Chunks = PushArray(PermArena, world_chunk, 100);
        World->ChunkSize = 100;
        
        uint32 ChunkStartX = (INT_MAX / 2);
        uint32 ChunkStartY = (INT_MAX / 2); 
        
        world_chunk *Chunk = World->Chunks;
        
        for(uint32 X = ChunkStartX; X < ChunkStartX + 10; ++X)
        {
            for(uint32 Y = ChunkStartY; Y < ChunkStartY + 10; ++Y)
            {
                Chunk->X = X;
                Chunk->Y = Y;
                Chunk++;
            }
        }
    }
    
    // TODO(stylia): make player controller mapping
    
    world_pos Pos1 = {5, 5, 0, 2, 0.2f, 0.3f};
    world_pos Pos2 = {5, 5, 0, 2, 2.2f, 2.3f};
    world_pos Pos3 = {5, 5, 0, 2, 8.7f, 9.1f};
    world_pos Pos4 = {5, 5, 0, 2, 0.0f, 0.0f};
    world_pos Pos5 = {5, 5, 0, 2, -0.75f, -0.5f};
    
    Pos1 = Canonicalize(World, Pos1);
    Pos2 = Canonicalize(World, Pos2);
    Pos3 = Canonicalize(World, Pos3);
    Pos4 = Canonicalize(World, Pos4);
    Pos5 = Canonicalize(World, Pos5);
    
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
    
    
    END_TIME_BLOCK(Sections_UpdateAndRender);
}

