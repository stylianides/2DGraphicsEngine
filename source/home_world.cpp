#include "home_world.h"
#include "home_intrinsics.h"

internal void
CanonicalizeCoord(world *World, int32 *BlockCoord, real32 *Coord)
{
    // TODO(stylia): Epsilon
    real32 BlockDim = World->BlockDimMeters;
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

internal v3
Difference(world *World, world_position A, world_position B)
{
    real32 BlockDim = World->BlockDimMeters;
    
    v3 Result; 
    
    Result.x = BlockDim*(A.Block.X - B.Block.X) + (A.Offset_.x - B.Offset_.x);
    Result.y = BlockDim*(A.Block.Y - B.Block.Y) + (A.Offset_.y - B.Offset_.y);
    Result.x = BlockDim*(A.Block.Z - B.Block.Z) + (A.Offset_.z - B.Offset_.z);
    
    return(Result);
}