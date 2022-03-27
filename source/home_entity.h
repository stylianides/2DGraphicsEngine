/* date = February 23rd 2022 4:49 pm */

#ifndef HOME_ENTITY_H
#define HOME_ENTITY_H

struct entity
{
    world_block Block;
    
    v3 P;
    v3 dP;
    
    v2 Thickness;
    
    v4 Colour;
};

#endif //HOME_ENTITY_H
