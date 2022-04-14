#include "engine_entity.h"

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
    
    if(Absolute(dP.x) > Absolute(dP.y))
    {
        Entity->FacingDirection = (dP.x > 0) ? 
        (FacingDirections_Right) : (FacingDirections_Left);
    }
    else
    {
        Entity->FacingDirection = (dP.y > 0) ?
        (FacingDirections_Back) : (FacingDirections_Front);
    }
    
    // NOTE(stylia): collision detection
    
    
    Offset(World, &Entity->Pos, Delta);
}

