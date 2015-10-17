//
//  An encapsulated physics engine
//  - Holds entities
//  - Makes sure entities, when moving, can't collide with other entities
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__entityholder__
#define __Bawk__entityholder__

#include <stdio.h>
#include <vector>
#include "entity.h"

class EntityHolder {
    // this is the baseworld. the baseworld is dimensionless so we must always check against this
    Entity* global_entity;
    // some data structure to hold all (nonbaseworld) entities in a efficient manner
    std::vector<Entity*> entities;
public:
    // given that an entity got updated, sees if this entity collided with anything else
    bool any_collided(Entity* updated);
};

#endif /* defined(__Bawk__entityholder__) */
