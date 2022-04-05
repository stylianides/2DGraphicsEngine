
#include "home_intrinsics.h"

internal void
DrawBitmap(engine_image *Buffer, bitmap_loaded Bitmap, v2 BitmapBottomLeft)
{
    int32 MinCornerX = RoundReal32ToInt32(BitmapBottomLeft.x) - Bitmap.AlignX;
    int32 MinCornerY = RoundReal32ToInt32(BitmapBottomLeft.y) - Bitmap.AlignY;
    
    int32 MaxCornerX = MinCornerX + Bitmap.Width;
    int32 MaxCornerY = MinCornerY + Bitmap.Height;
    
    int32 FrameBoundaryMinX = 0;
    int32 FrameBoundaryMaxX = Buffer->Width-1;
    
    int32 FrameBoundaryMinY = 0;
    int32 FrameBoundaryMaxY = Buffer->Height-1;
    
    int32 ImageStartX = (MinCornerX < FrameBoundaryMinX) ? 
    (FrameBoundaryMinX - MinCornerX) : (0);
    
    int32 ImageEndX = (MaxCornerX > FrameBoundaryMaxX) ? 
    (FrameBoundaryMaxX - MinCornerX) : (Bitmap.Width);
    
    int32 ImageStartY = (MinCornerY < FrameBoundaryMinY) ? 
    (FrameBoundaryMinY - MinCornerY) : (0); 
    
    int32 ImageEndY = (MaxCornerY > FrameBoundaryMaxY) ? 
    (FrameBoundaryMaxY - MinCornerY) : (Bitmap.Height);
    
    int32 ScreenX = LClamp0(MinCornerX);
    int32 ScreenY = LClamp0(MinCornerY);
    
    ScreenX = HClampN(ScreenX, FrameBoundaryMaxX);
    ScreenY = HClampN(ScreenY, FrameBoundaryMaxY);
    
    uint32 *Dest = (uint32 *)Buffer->Pixels + ScreenY*Buffer->Width + ScreenX;
    uint32 *Source = (uint32 *)Bitmap.Pixels;
    
    int32 DestX = 0;
    int32 DestY = 0;
    
    for(int32 Y = ImageStartY; Y < ImageEndY; ++Y)
    {
        uint32 *SourceRow = Source + Y*Bitmap.Width;
        uint32 *DestRow = Dest + DestY++*Buffer->Width;
        DestX = 0;
        
        for(int32 X = ImageStartX; X < ImageEndX; ++X)
        {
            uint32 *SourcePixel = SourceRow + X;
            uint32 *DestPixel = DestRow + DestX++;
            
            uint32 AlphaMask = 0xFF000000;
            uint8 Alpha = (uint8)((*SourcePixel & AlphaMask) >> 24);
            
            // TODO(stylia): Blended Alpha
            if(Alpha > 0)
            {
                *DestPixel = *SourcePixel;
            }
        }
    }
}

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
    
    for(int32 Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        for(int32 X = 0;
            X < Buffer->Width;
            ++X)
        {
            *(Pixel++) = Colour32;
        }
    }
}


