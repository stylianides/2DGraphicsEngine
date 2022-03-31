
#include "home_engine.h"
#include "home_render_group.cpp"
#include "home_entity.cpp"

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

bitmap_loaded LoadBitmap(char *Filename, debug_platform_read_file *ReadFile)
{
    bitmap_loaded Result = {};
    
    file_contents File = ReadFile(Filename);
    bitmap_header *BmpHeader = (bitmap_header *)File.Contents;
    Assert(BmpHeader->Compression == 3);
    
    uint32 *Pixels = (uint32 *)((uint8 *)BmpHeader + BmpHeader->BitmapOffset);
    
    int32 Width = BmpHeader->Width;
    int32 Height = BmpHeader->Height;
    
    uint32 RedMask = BmpHeader->RedMask;
    uint32 GreenMask = BmpHeader->GreenMask;
    uint32 BlueMask = BmpHeader->BlueMask;
    uint32 AlphaMask = ~(BmpHeader->RedMask | 
                         BmpHeader->GreenMask | 
                         BmpHeader->BlueMask);
    
    uint8 RedShift = BitScanForward32(RedMask);
    uint8 GreenShift = BitScanForward32(GreenMask);
    uint8 BlueShift = BitScanForward32(BlueMask);
    uint8 AlphaShift = BitScanForward32(AlphaMask);
    
    for(int32 Y = 0; Y < Width; ++Y)
    {
        for(int32 X = 0; X < Height; ++X)
        {
            uint32 *Pixel = Pixels + (Y*Width + X);
            
            uint32 SR = (*Pixel & RedMask)   >> RedShift;
            uint32 SG = (*Pixel & GreenMask) >> GreenShift;
            uint32 SB = (*Pixel & BlueMask)  >> BlueShift;
            uint32 SA = (*Pixel & AlphaMask) >> AlphaShift;
            
            *Pixel = (SA << 24) | (SR << 16)| (SG << 8) | (SB << 0);
        }
    }
    
    Result.Pixels = Pixels;
    Result.Width = Width;
    Result.Height = Height;
    Result.BitsPerPixel = BmpHeader->BitsPerPixel;
    
    return(Result);
}


#if DEBUG
engine_memory *DebugMemory;
#endif

extern "C"
ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender)
{
    engine_state *State = (engine_state *)Memory;
    
#if DEBUG
    DebugMemory = Memory;
    camera *DebugCamera = &State->DebugCamera;
#endif
    
    BEGIN_TIME_BLOCK(Sections_UpdateAndRender);
    
    memory_arena *PermArena = &State->PermanentArena;
    world *World = &State->World;
    
    real32 dt = Input->dt; // NOTE(stylia): Timestep
    
    
    
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
        State->PlayerSprite = LoadBitmap("./time_man_proto.bmp", Memory->DEBUGPlatformReadFile);
        
        State->World.BlockDimMeters = BLOCK_DIM_METERS;
        
        int32 BlockX = INT_MAX / 2;
        int32 BlockY = INT_MAX / 2;
        
        State->GameCamera.P = V3(0.0f, 0.0f, 0.0f);
        
        DebugCamera->Block.X = BlockX;
        DebugCamera->Block.Y = BlockY;
        DebugCamera->Block.Z = 0;
        DebugCamera->P = V3(0.0f, 0.0f, 0.0f);
        DebugCamera->RenderThickness = V2(5.0f, 5.0f);
        DebugCamera->ScreenMapping = V2((real32)(Buffer->Width / 2), (real32)(Buffer->Height / 2));
        
        State->Player.Block.X = BlockX;
        State->Player.Block.Y = BlockY;
        State->Player.P = V3(1.3f, 0.5f, 0.0f);
        
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
    
    
    
    for(uint32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ControllerIndex++)
    {
        engine_input_controller *Controller = Input->Controllers + ControllerIndex;
        
        if(Controller->IsConnected)
        {
            v3 ddP = {};
            
            // TODO(stylia): Total length has to be unit size, fix it
            if(Controller->Buttons.Up.Press)
            {
                ddP += V3(0.0f, 1.0f, 0.0f);
            }
            if(Controller->Buttons.Down.Press)
            {
                ddP += V3(0.0f, -1.0f, 0.0f);
            }
            if(Controller->Buttons.Left.Press)
            {
                ddP += V3(-1.0f, 0.0f, 0.0f);
            }
            if(Controller->Buttons.Right.Press)
            {
                ddP += V3(1.0f, 0.0f, 0.0f);
            }
            
            ddP *= 15;
            
            real32 DragRate = 2.0f;
            v3 Drag = DragRate*State->Player.dP;
            Drag.z = 0.0f;
            ddP -= Drag;
            
            v3 dP = State->Player.dP + dt*ddP;
            v3 Delta = ddP*(Square(dt) / 2.0f) +
                dt*State->Player.dP;
            
            State->Player.dP = dP;
            State->Player.P += Delta;
        }
    }
    
    // NOTE(stylia): Rendering
    
    v2 CameraTopLeft = DebugCamera->ScreenMapping - DebugCamera->RenderThickness;
    v2 CameraBottomRight = DebugCamera->ScreenMapping + DebugCamera->RenderThickness;
    DrawRectangle(Buffer, CameraTopLeft, CameraBottomRight, DebugCamera->RenderColour);
    
    
    // NOTE(stylia): Render players
    for(uint32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ControllerIndex++)
    {
        // TODO(stylia): Make player controller mapping
        engine_input_controller *Controller = Input->Controllers + ControllerIndex;
        
        if(Controller->IsConnected)
        {
            world_position CameraPosition = {DebugCamera->Block, DebugCamera->P};
            world_position PlayerPosition = {State->Player.Block, State->Player.P};
            
            v3 PlayerCameraDistance = Difference(World, CameraPosition, PlayerPosition);
            
            v2 PlayerCenter = DebugCamera->ScreenMapping - State->MetersToPixels * PlayerCameraDistance.xy;
            v2 PlayerTopLeft = PlayerCenter - State->Player.Thickness;
            v2 PlayerBottomRight = PlayerCenter + State->Player.Thickness;
            
            DrawBitmap(Buffer, State->PlayerSprite, PlayerBottomRight);
        }
    }
    
    END_TIME_BLOCK(Sections_UpdateAndRender);
}

