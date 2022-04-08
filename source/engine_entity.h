/* date = February 23rd 2022 4:49 pm */

#ifndef HOME_ENTITY_H
#define HOME_ENTITY_H

enum FacingDirections
{
    FacingDirections_Front = 0,
    FacingDirections_Back = 1,
    FacingDirections_Right = 2,
    FacingDirections_Left = 3,
    FacingDirections_Total = 4,
    
};

struct entity
{
    // TODO(stylia): think  about if we want id in entity system
    int32 ID;
    
    world_position Pos;
    
    v3 dP;
    
    FacingDirections FacingDirection;
    
    bitmap_loaded Sprite[FacingDirections_Total];
};

#endif //HOME_ENTITY_H
