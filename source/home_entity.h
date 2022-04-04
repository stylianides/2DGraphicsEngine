/* date = February 23rd 2022 4:49 pm */

#ifndef HOME_ENTITY_H
#define HOME_ENTITY_H


struct entity
{
    // TODO(stylia): think  about if we want id in entity system
    int32 ID;
    
    world_position Pos;
    
    v3 dP;
    
    bitmap_loaded Sprite;
};

#endif //HOME_ENTITY_H
