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
    CanonicalizeCoord(World, &Pos->Block.X, &Pos->Offset_.x);
    CanonicalizeCoord(World, &Pos->Block.Y, &Pos->Offset_.y);
    CanonicalizeCoord(World, &Pos->Block.Z, &Pos->Offset_.z);
}

internal void 
Offset(world *World, world_position *P, v3 Delta)
{
    P->Offset_ += Delta;
    Canonicalize(World, P);
}

internal v3
Distance(world *World, world_position A, world_position B)
{
    real32 BlockDim = World->BlockDimMeters;
    
    v3 Result = {};
    
    Result.x = BlockDim*(A.Block.X - B.Block.X) + (A.Offset_.x - B.Offset_.x);
    Result.y = BlockDim*(A.Block.Y - B.Block.Y) + (A.Offset_.y - B.Offset_.y);
    Result.z = BlockDim*(A.Block.Z - B.Block.Z) + (A.Offset_.z - B.Offset_.z);
    
    return(Result);
}

inline world_position
GetWorldPosition(entity *Entity)
{
    world_position Result = {Entity->Block, Entity->P};
    
    return(Result);
}