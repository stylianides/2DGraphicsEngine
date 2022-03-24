#include "home_world.h"
#include "home_intrinsics.h"



internal world_pos Canonicalize(world *World, world_pos Pos)
{
    world_pos Result = Pos;
    
    int32 TileOffsetX = RoundReal32ToInt32(Pos.Rel.x / World->TileDim);
    int32 TileOffsetY = RoundReal32ToInt32(Pos.Rel.y / World->TileDim);
    
    Result.TileX += TileOffsetX; 
    Result.TileY += TileOffsetY; 
    
    if(TileOffsetX > 0)
    {
        Result.Rel.x =  -1.0f + (Pos.Rel.x - TileOffsetX * World->TileDim);
    }
    else if(TileOffsetX < 0)
    {
        Result.Rel.x =  1.0f - (Pos.Rel.x - TileOffsetX * World->TileDim);
    }
    
    if(TileOffsetY > 0)
    {
        Result.Rel.y = -1.0f + (Pos.Rel.y - TileOffsetX * World->TileDim);
    }
    else if(TileOffsetY < 0)
    {
        Result.Rel.y = 1.0f - (Pos.Rel.y - TileOffsetX * World->TileDim);
    }
    
    int32 ChunkOffsetX = Result.TileX / CHUNK_DIM_X;
    int32 ChunkOffsetY = Result.TileY / CHUNK_DIM_Y;
    
    Assert(ChunkOffsetX > -2 && ChunkOffsetX < 2);
    Assert(ChunkOffsetY > -2 && ChunkOffsetY < 2);
    
    if(ChunkOffsetX)
    {
        Result.ChunkX += ChunkOffsetX;
        Result.TileX = Result.TileX - ChunkOffsetX * CHUNK_DIM_X;
    }
    
    if(ChunkOffsetY)
    {
        Result.ChunkY += ChunkOffsetY;
        Result.TileY = Result.TileY - ChunkOffsetY * CHUNK_DIM_Y;
    }
    
    return(Result);
}