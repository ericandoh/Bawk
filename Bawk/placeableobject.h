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
class Game;
class SuperObject;

class PlaceableObject {
protected:
    bool mvp_set;
    fmat4 mvp;
    void set_mvp(ivec3 bounds);
public:
    // if independent, creates a new entity whenever placed into another object
    bool independent;
    PlaceableObject();
    // sets blocks into another superobject
    bool set_blocks(Game* game);
    virtual bool set_blocks(Player* player, World* world, SuperObject* object) = 0;
};

#endif /* defined(__Bawk__placeableobject__) */
