//
//  The basic building block for all tangible objects in our universe
//  - Has a position, and can move around if permitted
//  - Can rotate on both axises
//  - Has some bounds, and other objects can't share space within its bounds
//
//  Used by:
//  - A lot of shit
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

#ifndef __Bawk__entity__
#define __Bawk__entity__

#include <stdio.h>
#include "basic_types.h"

class Entity {
protected:
    // position of the superobject, in RWC
    fvec3 pos;
    // up/dir vectors. these should be normalized
    fvec3 up;
    fvec3 dir;
public:
    // the bounding box over contents of all chunks, in OAC
    // public for convenience of access
    fvec3 lower_bound, upper_bound;
    Entity();
    Entity(fvec3 p, fvec3 u, fvec3 d, fvec3 lower, fvec3 upper);
    // internal function to transform RWC xyz to OAC src
    void transform_into_my_coordinates(fvec3* src, float x, float y, float z);
    // internal function to transform OAC xyz to RWC src
    void transform_into_world_coordinates(fvec3* src, float x, float y, float z);
    // movement methods
    void move_forward();
    void move_backward();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    fvec3* get_pos();
    bool set_pos(fvec3 to);

    bool collides_with(Entity* other);
    // override this with your own int for up to what class of Entities you can handle collision
    // detection for
    virtual int get_collision_priority();
    // general method for collision detection vs other entities
    virtual bool check_collision_vs(Entity* other);
    // method for collision detection against base class entities ONLY
    bool collides_with_entity(Entity* other);
    
    // check if a bounding box (OAC) (lower, upper) intersects with my own bounds
    virtual bool intersects_with_my_bounds(fvec3 lower_corner, fvec3 upper_corner);
};

#endif /* defined(__Bawk__entity__) */
