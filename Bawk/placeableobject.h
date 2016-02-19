//
//  placeableobject.h
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__placeableobject__
#define __Bawk__placeableobject__

#include <stdio.h>
#include "basic_types.h"

class Player;
class World;
class SuperObject;

class PlaceableObject {
protected:
    // --- Camera MVP related ---
    bool mvp_set;
    fmat4 mvp;
    void calculate_mvp(ivec3 bounds);
    void set_mvp();
public:
    // if independent, creates a new entity whenever placed into another object
    bool independent;
    PlaceableObject();
    virtual bool set_blocks(Player* player, SuperObject* object) = 0;
};

#endif /* defined(__Bawk__placeableobject__) */
