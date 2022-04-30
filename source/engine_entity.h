/* date = February 23rd 2022 4:49 pm */

#ifndef HOME_ENTITY_H
#define HOME_ENTITY_H

enum entity_type
{
    EntityType_Null,
    EntityType_Player,
    EntityType_Wall,
};

struct entity
{
    // TODO(stylia): think  about if we want id in entity system
    int32 Id;
    entity_type Type;
    v3 Dim;
    
    world_block Block;
    v3 P;
    v3 dP;
    
    
    FacingDirections FacingDirection;
    bitmap_loaded Sprite[FacingDirections_Total];
    
};

#endif //HOME_ENTITY_H
