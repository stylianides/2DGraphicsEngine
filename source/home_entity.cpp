#include "home_entity.h"

internal void
MoveEntity(world *World, entity *Entity, v3 Delta)
{
    world_position Pos = {};
    
    Pos.Block = Entity->Block;
    Pos.Offset_ += Delta;
    Pos = Canonicalize(World, Pos);
    
    Entity->Block = Pos.Block;
    Entity->P = Pos.Offset_;
}