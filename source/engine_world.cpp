#include "engine_world.h"
#include "engine_intrinsics.h"

internal void
CanonicalizeCoord(world *World, int32 *BlockCoord, real32 *Coord)
{
    // TODO(stylia): Epsilon
    real32 BlockDim = World->BlockDimMeters;
    real32 HalfBlockDim = 0.5f*BlockDim;
    
    int32 BlockOffset = RoundReal32ToInt32(*Coord / BlockDim);
    
    *BlockCoord = *BlockCoord + BlockOffset;
    if(BlockOffset != 0)
    {
        *Coord -= (BlockOffset*BlockDim);
    }
}

internal void
Canonicalize(world *World, world_position *Pos)
{
    CanonicalizeCoord(World, &Pos->Block.X, &Pos->P.x);
    CanonicalizeCoord(World, &Pos->Block.Y, &Pos->P.y);
    CanonicalizeCoord(World, &Pos->Block.Z, &Pos->P.z);
}

internal v3
Difference(world *World, world_position A, world_position B)
{
    real32 BlockDim = World->BlockDimMeters;
    
    v3 Result = {};
    
    Result.x = BlockDim*(A.Block.X - B.Block.X) + (A.P.x - B.P.x);
    Result.y = BlockDim*(A.Block.Y - B.Block.Y) + (A.P.y - B.P.y);
    Result.z = BlockDim*(A.Block.Z - B.Block.Z) + (A.P.z - B.P.z);
    
    return(Result);
}


