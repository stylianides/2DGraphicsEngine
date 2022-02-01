#include "home_engine.h"

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
    // NOTE(stylia): Try the HotLoading by changing Colours!
    RenderColour(Buffer, 0xFFAAAAAA);
}

