#include "engine_entity.h"
#include "engine_world.h"

// TODO(stylia): Assuming that Simposition begins from INT_MAX / 2 - 3
v3 GetSimPosition(world *World, world_position P)
{
    v3 Result = {};
    
    int32 RefBlockX = (INT_MAX / 2) - 3;
    int32 RefBlockY = (INT_MAX / 2) - 3;
    int32 RefBlockZ = 0;
    
    world_position RefP = {RefBlockX,
        RefBlockY,
        RefBlockZ,
        V3(0.0f, 0.0f, 0.0f)};
    
    Result = Distance(World, P, RefP);
    
    return(Result);
}

// TODO(stylia): remove state from this, this is temporary
internal void
MoveEntity(world *World, entity *MovingEntity, 
           v3 ddP, real32 dt, engine_state *State)
{
    v3 dP = MovingEntity->dP;
    v3 P = GetSimPosition(World, {MovingEntity->Block, MovingEntity->P});
    
    // TODO(stylia): ODE
    real32 DragRate = 2.0f;
    v3 Drag = DragRate * MovingEntity->dP;
    Drag.z = 0.0f;
    ddP -= Drag;
    
    v3 Delta = 0.5f*ddP*Square(dt) + dt*dP;
    v3 Prime(dP) = dP + dt*ddP;
    v3 Prime(P) = P + Delta;
    
    
    // TODO(stylia): Distance Limit
    
    // NOTE(stylia): collision detection
    
    entity *Wall = &State->Wall;
    
    bool32 Collides = false;
    
    // TODO(stylia): Positions should be in reference to a wider area
    //               the simulation area
    v3 WallSP = GetSimPosition(World, {Wall->Block, Wall->P});
    
    rect3 PlayerRect = RectCenterHalfDim(Prime(P), MovingEntity->Dim);
    rect3 WallRect = RectCenterHalfDim(WallSP, State->Wall.Dim);
    
    if(Intersect(PlayerRect, WallRect))
    {
        Collides = true;
    }
    
    if(!Collides)
    {
        MovingEntity->P += Delta;
        MovingEntity->dP += dt*ddP;
    }
    else
    {
        v3 WallNormal = {};
        
        // NOTE(stylia): Minkowski Collision
        v3 MovingEntityHalfDim = 0.5f*MovingEntity->Dim;
        
        rect3 MinkowskiVolume;
        MinkowskiVolume.Min = WallRect.Min - MovingEntityHalfDim;
        MinkowskiVolume.Max = WallRect.Max + MovingEntityHalfDim;
        
        // NOTE(stylia): From the left
        if(P.x > MinkowskiVolume.Max.x && 
           Prime(P).x <= MinkowskiVolume.Max.x)
        {
            WallNormal = V3(1.0f, 0.0f, 0.0f);
        }
        // NOTE(stylia): From the right
        else if(P.x < MinkowskiVolume.Min.x && 
                Prime(P).x >= MinkowskiVolume.Min.x)
        {
            WallNormal = V3(-1.0f, 0.0f, 0.0f);
        }
        // NOTE(stylia): From above
        else if(P.y > MinkowskiVolume.Max.y && 
                Prime(P).y <= MinkowskiVolume.Max.y)
        {
            WallNormal = V3(0.0f, 1.0f, 0.0f);
        }
        // NOTE(stylia): From below
        else if(P.y < MinkowskiVolume.Min.y && 
                Prime(P).y >= MinkowskiVolume.Min.y)
        {
            WallNormal = V3(0.0f, -1.0f, 0.0f);
        }
        
        // NOTE(stylia): Reflect the vector
        MovingEntity->dP = Prime(dP) - 2*Inner(Prime(dP), WallNormal)*WallNormal;
    }
    
    if(Absolute(dP.x) > Absolute(dP.y))
    {
        MovingEntity->FacingDirection = (dP.x > 0) ? 
        (FacingDirections_Right) : (FacingDirections_Left);
    }
    else
    {
        MovingEntity->FacingDirection = (dP.y > 0) ?
        (FacingDirections_Back) : (FacingDirections_Front);
    }
}

