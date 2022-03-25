#include "home_world.h"
#include "home_intrinsics.h"

internal void
CanonicalizeCoord(world *World, int32 *BlockCoord, real32 *Coord)
{
    // TODO(stylia): Epsilon
    real32 BlockDim = World->BlockDim;
    real32 HalfBlockDim = 0.5f*BlockDim;
    
    int32 BlockOffset = RoundReal32ToInt32(*Coord / BlockDim);
    
    *BlockCoord += BlockOffset;
    if(BlockOffset > 0)
    {
        *Coord += -(BlockOffset*BlockDim);
    }
    else if (BlockOffset < 0)
    {
        *Coord += (BlockOffset*BlockDim);
    }
}

internal world_position
Canonicalize(world *World, world_position Pos)
{
    world_position Result = Pos;
    
    CanonicalizeCoord(World, &Result.Block.X, &Result.Offset_.x);
    CanonicalizeCoord(World, &Result.Block.Y, &Result.Offset_.y);
    CanonicalizeCoord(World, &Result.Block.Z, &Result.Offset_.z);
    
    return(Result);
}