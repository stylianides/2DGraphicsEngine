
#include "home_intrinsics.h"

internal void
DrawRectangle(engine_image *Buffer, v2 Min, v2 Max, v4 Colour)
{
    int32 MinX = RoundReal32ToInt32(Min.x);
    int32 MaxX = RoundReal32ToInt32(Max.x);
    int32 MinY = RoundReal32ToInt32(Min.y);
    int32 MaxY = RoundReal32ToInt32(Max.y);
    
    MinX = (MinX < 0) ? 0 : MinX;
    MaxX = (MaxX > (int32)Buffer->Width) ? Buffer->Width : MaxX;
    
    MinY = (MinY < 0) ? 0 : MinY;
    MaxY = (MaxY > (int32)Buffer->Height) ? Buffer->Height : MaxY;
    
    uint32 Colour32 = ((RoundRealToUInt32(Colour.r * 255.0f) << 16) |
                       (RoundRealToUInt32(Colour.g * 255.0f) << 8) |
                       (RoundRealToUInt32(Colour.b * 255.0f) << 0));
    
    for(int32 Y = MinY;
        Y < MaxY;
        ++Y)
    {
        uint32 *Row = (uint32 *)Buffer->Pixels + Buffer->Width*Y;
        
        for(int32 X = MinX;
            X < MaxX;
            ++X)
        {
            uint32 *Pixel = Row + X;
            *(Pixel) = Colour32;
        }
    }
}

internal void
DrawRectangleOutline(engine_image *Buffer, v2 Min, v2 Max, 
                     uint32 Th, v4 Colour)
{
    DrawRectangle(Buffer, V2(Min.x - Th, Min.y - Th), V2(Max.x + Th, Min.y + Th), Colour);
    DrawRectangle(Buffer, V2(Min.x - Th, Min.y - Th), V2(Min.x + Th, Max.y + Th), Colour);
    DrawRectangle(Buffer, V2(Min.x - Th, Max.y - Th), V2(Max.x + Th, Max.y + Th), Colour);
    DrawRectangle(Buffer, V2(Max.x - Th, Min.y - Th), V2(Max.x + Th, Max.y + Th), Colour);
}

internal void 
ClearScreen(engine_image *Buffer, v4 Colour)
{
    uint32 *Pixel = (uint32 *)Buffer->Pixels;
    
    uint32 Colour32 = ((RoundRealToUInt32(Colour.r * 255.0f) << 16) |
                       (RoundRealToUInt32(Colour.g * 255.0f) << 8) |
                       (RoundRealToUInt32(Colour.b * 255.0f) << 0));
    
    for(uint32 Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        for(uint32 X = 0;
            X < Buffer->Width;
            ++X)
        {
            *(Pixel++) = Colour32;
        }
    }
}
