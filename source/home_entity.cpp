#include "home_entity.h"

internal void Offset(world *World, world_position *Pos, v3 Delta)
{
    Pos->P += Delta;
    Canonicalize(World, Pos);
}

internal void
MoveEntity(world *World, entity *Entity, v3 ddP, real32 dt)
{
    real32 DragRate = 2.0f;
    v3 Drag = DragRate * Entity->dP;
    Drag.z = 0.0f;
    ddP -= Drag;
    
    v3 Delta = ddP*(Square(dt) / 2.0f) + dt*Entity->dP;
    
    v3 dP = Entity->dP + dt*ddP;
    Entity->dP = dP;
    
    Offset(World, &Entity->Pos, Delta);
}

