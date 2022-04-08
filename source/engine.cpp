
#include "engine.h"
#include "engine_render_group.cpp"
#include "engine_entity.cpp"

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

bitmap_loaded LoadBitmap(char *Filename, debug_platform_read_file *ReadFile, 
                         int32 AlignX = 0, int32 AlignY = 0)
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
    Result.AlignX = AlignX;
    Result.AlignY = AlignY;
    
    return(Result);
}


#if DEBUG
engine_memory *DebugMemory;
#endif

extern "C"
ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender)
{
    BEGIN_TIME_BLOCK(Sections_UpdateAndRender);
    
    engine_state *State = (engine_state *)Memory;
    
#if DEBUG
    DebugMemory = Memory;
    camera *DebugCamera = &State->DebugCamera;
#endif
    
    memory_arena *PermArena = &State->PermanentArena;
    world *World = &State->World;
    real32 dt = Input->dt;
    
    ClearScreen(Buffer, State->BackDropColour);
    
    // NOTE(stylia): Assume that the first bytes of memory are the engine state
    
    
    if(!Memory->IsMemoryInitialized)
    {
        InitializeArena(PermArena,
                        Memory->PermanentMemorySize - sizeof(engine_state),
                        (uint8 *)Memory + sizeof(engine_state));
        
        State->RG.MetersToPixels = 40.0f;
        State->BackDropColour = V4(0.3f, 0.3f, 0.3f, 0.3f);
        
        // TODO(stylia): make art for back and front
        State->Player.Sprite[FacingDirections_Right] = LoadBitmap("./time_man_proto_right.bmp", Memory->DEBUGPlatformReadFile, 54, 30);
        State->Player.Sprite[FacingDirections_Left] = LoadBitmap("./time_man_proto_left.bmp", Memory->DEBUGPlatformReadFile, 54, 30);
        State->Player.Sprite[FacingDirections_Front] = LoadBitmap("./time_man_proto_front.bmp", Memory->DEBUGPlatformReadFile, 54, 30);
        State->Player.Sprite[FacingDirections_Back] = LoadBitmap("./time_man_proto_back.bmp", Memory->DEBUGPlatformReadFile, 54, 30);
        
        State->World.BlockDimMeters = BLOCK_DIM_METERS;
        
        int32 BlockX = INT_MAX / 2;
        int32 BlockY = INT_MAX / 2;
        
        State->GameCamera.Pos.P = V3(0.0f, 0.0f, 0.0f);
        
        DebugCamera->Pos.Block = {BlockX, BlockY, 0};
        DebugCamera->Pos.P = V3(0.0f, 0.0f, 0.0f);
        DebugCamera->RenderThickness = V2(5.0f, 5.0f);
        DebugCamera->ScreenMapping = V2((real32)(Buffer->Width / 2), (real32)(Buffer->Height / 2));
        
        State->Player.Pos.Block = {BlockX, BlockY, 0};
        State->Player.Pos.P = V3(1.3f, 0.5f, 0.0f);
        
        // TODO(stylia): change this
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
            // NOTE(stylia): StickX, StickY are normalized
            real32 dX = Controller->StickX;
            real32 dY = Controller->StickY;
            
            if(Controller->Buttons.Up.Press)
            {
                dY = 1.0f;
            }
            if(Controller->Buttons.Down.Press)
            {
                dY = -1.0f;
            }
            if(Controller->Buttons.Left.Press)
            {
                dX = -1.0f;
            }
            if(Controller->Buttons.Right.Press)
            {
                dX = 1.0f;
            }
            
            v3 ddP = {};
            
            if((Square(dX) + Square(dY)) <= 1)
            {
                ddP = V3(dX, dY, 0.0f);
            }
            else
            {
                real32 Hypotenuse = Sqrt(Square(dX) + Square(dY));
                
                real32 dXPrime = dX / Hypotenuse;
                real32 dYPrime = dY / Hypotenuse;
                
                ddP = V3(dXPrime, dYPrime, 0.0f);
            }
            ddP *= 15;
            
            MoveEntity(World, &State->Player, ddP, dt);
        }
    }
    
    // NOTE(stylia): Rendering
    
    // TODO(stylia): Facing directions for sprite
    
    v2 CameraTopLeft = DebugCamera->ScreenMapping - DebugCamera->RenderThickness;
    v2 CameraBottomRight = DebugCamera->ScreenMapping + DebugCamera->RenderThickness;
    DrawRectangle(Buffer, CameraTopLeft, CameraBottomRight, DebugCamera->RenderColour);
    
    
    // NOTE(stylia): Render players
    
    
    for(uint32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ControllerIndex++)
    {
        // TODO(stylia): Render entities on a camera bound only
        // TODO(stylia): Query entities on a set area
        engine_input_controller *Controller = Input->Controllers + ControllerIndex;
        
        if(Controller->IsConnected)
        {
            world_position CameraPosition = DebugCamera->Pos;
            world_position PlayerPosition = State->Player.Pos;
            
            int32 FacingDirection = State->Player.FacingDirection;
            
            v3 PlayerCameraDistance = Difference(World, CameraPosition, PlayerPosition);
            
            v2 PlayerCenter = DebugCamera->ScreenMapping - State->RG.MetersToPixels * PlayerCameraDistance.xy;
            
            DrawRectangle(Buffer, PlayerCenter, PlayerCenter + V2(1.0f, 1.0f), V4(1.0f, 0.0f, 1.0f, 1.0f));
            DrawBitmap(Buffer, State->Player.Sprite[FacingDirection], PlayerCenter);
        }
    }
    
    END_TIME_BLOCK(Sections_UpdateAndRender);
}