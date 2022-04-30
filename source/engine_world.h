
#ifndef HOME_WORLD_H
#define HOME_WORLD_H

#define BLOCK_DIM_METERS 10

struct world_block
{
    int32 X;
    int32 Y;
    int32 Z;
};

struct world_position
{
    world_block Block;
    v3 Offset_; // NOTE(stylia): From the block center
};

struct world
{
    real32 BlockDimMeters;
    
    world_block Blocks[4096];
};             

#endif //HOME_WORLD_H
