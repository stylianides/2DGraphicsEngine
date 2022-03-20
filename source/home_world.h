
#ifndef HOME_WORLD_H
#define HOME_WORLD_H

struct world_pos
{
    uint64 ChunkX;
    uint64 ChunkY;
    
    uint8 TileX;
    uint8 TileY;
    
    real32 X;
    real32 Y;
};

struct world_tile
{
    uint8 TileX;
    uint8 TileY;
    
    real32 X;
    real32 Y;
};

struct world_chunk
{
    uint64 X;
    uint64 Y;
    
    world_tile Tiles[4][4];
};

struct world
{
    real32 TileEdgeInMeters;
    
    // TODO(stylia): sparse storage, not static
    world_chunk Chunks[16];
};             

#endif //HOME_WORLD_H
