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
    ~EntityHolder();
    void set_global_entity(Entity* entity);
    void add_entity(Entity* entity);
    void remove_entity(Entity* entity);
    void step();
    void render(fmat4* transform);
    void update_chunks(fvec3* old_pos, fvec3* new_pos);
    // checks if an entity not being held will collide if added
    bool collides_with(Entity* entity);
};

#endif /* defined(__Bawk__entityholder__) */
