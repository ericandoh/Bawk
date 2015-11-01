//
//  worldevent.h
//  Bawk
//
//  Created by Eric Oh on 10/31/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__worldevent__
#define __Bawk__worldevent__

#include <stdio.h>
#include "basic_types.h"

class World;

class WorldEvent {
public:
    uint16_t eid;
    long expiration;
    bool expires;
    WorldEvent();
    WorldEvent(long duration);
    virtual void action(World* world) = 0;
};



void make_debug_event(World* world, long duration);
void make_spawn_event(World* world, uint16_t recipe, fvec3 pos);
void make_spawn_ore_event(World* world, uint16_t type, fvec3 pos, int radius);

void free_world_event(WorldEvent* event);

#endif /* defined(__Bawk__worldevent__) */
