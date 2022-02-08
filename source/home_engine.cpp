#include "home_engine.h"



internal void
DrawRectangleUnsafe(engine_image *Buffer, 
                    real32 MinX, real32 MaxX,
                    real32 MinY, real32 MaxY,
                    uint32 Colour)
{
    for(uint32 Y = (uint32)MinY;
        Y < MaxY;
        ++Y)
    {
        uint32 *Row = (uint32 *)Buffer->Pixels + Buffer->Width*Y;
        for(uint32 X = (uint32)MinX;
            X < MaxX;
            ++X)
        {
            uint32 *Pixel = Row + X;
            *(Pixel) = Colour;
        }
    }
}

internal void 
RenderColour(engine_image *Buffer, uint32 Colour)
{
    uint32 *Pixel = (uint32 *)Buffer->Pixels;
    
    for(uint32 Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        for(uint32 X = 0;
            X < Buffer->Width;
            ++X)
        {
            *(Pixel++) = Colour;
        }
    }
}

extern "C"
ENGINE_OUTPUT_SOUND(EngineOutputSound)
{
    real32 SecondsPerFlip = 1.0f / Sound->Hz;
    int32 SamplesPerFlip = (int32)(Sound->SamplesPerSecond * SecondsPerFlip);
    
    uint16 Volume = 130;
    
    uint32 BytesWritten = 0;
    uint16 *Current = (uint16 *)Sound->Samples;
    
    while(BytesWritten < Sound->SampleBufferSize)
    {
        *Current++ = (int16)(sinf(State->tSin)*Volume); // NOTE(stylia): first channel
        *Current++ = (int16)(sinf(State->tSin)*Volume); // NOTE(stylia): second channel
        
        BytesWritten += 4;
        
        State->tSin += 2.0f*Pi32*(1.0f/(real32)Sound->Hz);
        if(State->tSin > 2.0f*Pi32)
        {
            State->tSin -= 2.0f*Pi32;
        }
    }
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
    
    if(Input->Keyboard.Buttons.Right.Press)
    {
        State->P.x += 1;
    }
    
    if(Input->Keyboard.Buttons.Left.Press)
    {
        State->P.x -= 1;
    }
    
    if(Input->Keyboard.Buttons.Up.Press)
    {
        State->P.y -= 1;
    }
    
    if(Input->Keyboard.Buttons.Down.Press)
    {
        State->P.y += 1;
    }
    
    int32 PlayerRadiusX = 20;
    int32 PlayerRadiusY = 20;
    
    // NOTE(stylia): Try the HotLoading by changing Colours!
    RenderColour(Buffer, 0xFFFFFFFF);
    
    DrawRectangleUnsafe(Buffer, 
                        State->P.x - PlayerRadiusX, State->P.x + PlayerRadiusY,
                        State->P.y - PlayerRadiusY, State->P.y + PlayerRadiusY,
                        0xFF00FF00);
    
    file_contents Test =  State->DEBUGPlatformReadFile("C:\\Work\\advent_of_code\\2015\\problem01\\part1.cpp");
    State->DEBUGPlatformWriteFile("C:\\Work\\advent_of_code\\2015\\problem01\\part3.cpp", Test.Contents, Test.Size);
    State->DEBUGPlatformFreeFile(Test);
    
    int a = 1;
}

