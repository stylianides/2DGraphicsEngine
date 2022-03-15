/* date = February 23rd 2022 4:49 pm */

#ifndef HOME_ENTITY_H
#define HOME_ENTITY_H

struct entity_player
{
    bool32 Active;
    
    v2 P;
    v2 dP;
    
    v2 Dim;
    
    uint32 Color;
};

#endif //HOME_ENTITY_H
