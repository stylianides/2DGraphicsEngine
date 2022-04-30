#include "engine_entity.h"
#include "engine_world.h"


// TODO(stylia): remove state from this, this is temporary
internal void
MoveEntity(world *World, entity *Entity, 
           v3 ddP, real32 dt, engine_state *State)
{
    v3 dP = Entity->dP;
    v3 P = Entity->P;
    
    // TODO(stylia): ODE
    real32 DragRate = 2.0f;
    v3 Drag = DragRate * Entity->dP;
    Drag.z = 0.0f;
    ddP -= Drag;
    
    v3 Delta = 0.5f*ddP*Square(dt) + dt*dP;
    v3 Prime(dP) = dP + dt*ddP;
    v3 Prime(P) = P + Delta;
    
    
    // TODO(stylia): Distance Limit?
    
    
    
    // NOTE(stylia): collision detection
    
    bool32 Collides = false;
    
    // TODO(stylia): problem when collision happens in different tiles
    
    world_position WallWP = GetWorldPosition(&State->Wall);
    
    // TODO(stylia): This is temporary
    rect3 PlayerRect = RectCenterDim(Prime(P), 1.0f);
    rect3 WallRect = RectCenterDim(WallWP.Offset_, 1.0f);
    
    if(Intersect(PlayerRect, WallRect))
    {
        Collides = true;
    }
    
    if(!Collides)
    {
        Entity->P += Delta;
        Entity->dP += dt*ddP;
    }
    
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
}

