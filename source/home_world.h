
#ifndef HOME_WORLD_H
#define HOME_WORLD_H

#define CHUNK_DIM_X 4
#define CHUNK_DIM_Y 4

struct world_pos
{
    uint64 ChunkX;
    uint64 ChunkY;
    
    uint32 TileX;
    uint32 TileY;
    
    v2 Rel;
};

struct world_tile
{
    uint32 TileX;
    uint32 TileY;
};

struct world_chunk
{
    uint64 X;
    uint64 Y;
    
    world_tile Tiles[CHUNK_DIM_Y][CHUNK_DIM_X];
};

struct world
{
    real32 TileDim;
    
    // TODO(stylia): sparse storage, not static, hash table
    world_chunk *Chunks;
    uint32 ChunkSize;
};             

#endif //HOME_WORLD_H
