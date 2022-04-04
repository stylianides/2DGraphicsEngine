/* date = January 28th 2022 7:44 pm */

#ifndef HOME_ENGINE_H
#define HOME_ENGINE_H

#include "home_engine_platform.h"
#include "home_engine_math.h"
#include "home_world.cpp"

struct bitmap_loaded
{
    uint32 *Pixels;
    
    int32 Width;
    int32 Height;
    
    uint16 BitsPerPixel;
    
    int32 AlignX;
    int32 AlignY;
};

#include "home_entity.h"
#include "home_render_group.h"

struct memory_arena
{
    memory_index Size;
    memory_index Used;
    uint8 *Base;
};

internal void
InitializeArena(memory_arena *Arena, memory_index Size, uint8 *Base)
{
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->Base = Base;
    Assert(Arena->Base);
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, type, Amount) (type *)PushSize_(Arena, Amount*sizeof(type))

internal void *PushSize_(memory_arena *Arena, memory_index Size)
{
    Assert(Arena->Used + Size <= Arena->Size);
    
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return(Result);
}

#pragma pack(push, 1)
struct bitmap_header
{
    uint16 FileType;       /* File type, always 4D42h ("BM") */
    uint32 FileSize;       /* Size of the file in bytes */
    uint16 Reserved1;      /* Always 0 */
    uint16 Reserved2;      /* Always 0 */
    uint32 BitmapOffset;   /* Starting position of image data in bytes */
    uint32 Size;            /* Size of this header in bytes */
    int32 Width;           /* Image width in pixels */
    int32 Height;          /* Image height in pixels */
    uint16 Planes;          /* Number of color planes */
	uint16 BitsPerPixel;    /* Number of bits per pixel */
	uint32 Compression;     /* Compression methods used */
	uint32 SizeOfBitmap;    /* Size of bitmap in bytes */
	int32 HorzResolution;  /* Horizontal resolution in pixels per meter */
	int32 VertResolution;  /* Vertical resolution in pixels per meter */
	uint32 ColorsUsed;      /* Number of colors in the image */
	uint32 ColorsImportant; /* Minimum number of important colors */
    
    uint32 RedMask;         /* Mask identifying bits of red component */
    uint32 GreenMask;       /* Mask identifying bits of green component */
    uint32 BlueMask;        /* Mask identifying bits of blue component */
};
#pragma pack(pop)

struct camera
{
    world_position Pos;
    
    v2 ScreenMapping;
    
    v2 RenderThickness;
    v4 RenderColour;
};

struct engine_state
{
    memory_arena PermanentArena;
    
    // TODO(stylia): Work on this
    memory_arena TransientArena;
    
    world World;
    
    camera GameCamera;
#if DEBUG
    camera DebugCamera;
#endif
    
    // NOTE(stylia): Maps controller with an entity ID
    entity Player;
    uint32 ControllerEntityMapping[MAX_PLAYERS + 1];
    
    // NOTE(stylia): Rendering
    render_group RG;
    v4 BackDropColour;
    
    // NOTE(stylia): Audio
    real32 tSin;
};


#endif //HOME_ENGINE_H
