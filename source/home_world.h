
#ifndef HOME_WORLD_H
#define HOME_WORLD_H

#define BLOCK_DIM_METERS 4

struct world_block
{
    int32 X;
    int32 Y;
    int32 Z;
};

struct world_position
{
    world_block Block;
    
    // NOTE(stylia): Offset from center of block
    //               Don't change this directly, use Offset
    v3 P;
};

struct world
{
    real32 BlockDimMeters;
    
    world_block Blocks[4096];
};             

#endif //HOME_WORLD_H
