#include "home_engine.h"



internal void
DrawRectangleUnsafe(engine_image *Buffer, 
                    rect2 Rect, uint32 Colour)
{
    for(uint32 Y = (uint32)Rect.Min.y;
        Y < Rect.Max.y;
        ++Y)
    {
        uint32 *Row = (uint32 *)Buffer->Pixels + Buffer->Width*Y;
        for(uint32 X = (uint32)Rect.Min.x;
            X < Rect.Max.x;
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
    
}

extern "C"
ENGINE_UPDATE_AND_RENDER(EngineUpdateAndRender)
{
    if(!State->IsMemoryInitialized)
    {
        /*State->P.Min = V2(5.0f, 5.0f);
        State->P.Max = V2(40.0f, 40.0f);*/
        
        State->IsMemoryInitialized = true;
    }
    
    // NOTE(stylia): This is bad code for testing save state and input loops
    /*
    if(Input->Keyboard.Buttons.Right.Press)
    {
        State->P.Min.x += 5;
        State->P.Max.x += 5;
    }
    
    if(Input->Keyboard.Buttons.Left.Press)
    {
        State->P.Min.x -= 5;
        State->P.Max.x -= 5;
    }
    
    if(Input->Keyboard.Buttons.Up.Press)
    {
        State->P.Min.y -= 5;
        State->P.Max.y -= 5;
    }
    
    if(Input->Keyboard.Buttons.Down.Press)
    {
        State->P.Min.y += 5;
        State->P.Max.y += 5;
    }
    */
    // NOTE(stylia): Try the HotLoading by changing Colours!
    RenderColour(Buffer, 0xFFFFFFFF);
    
    file_contents Test =  State->DEBUGPlatformReadFile("C:\\Work\\advent_of_code\\2015\\problem01\\part1.cpp");
    int a = 1;
}

