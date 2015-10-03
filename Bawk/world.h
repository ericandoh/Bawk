//
//  Represents all world data
//  - Holds information about (all) players, blocks, chunks in this world
//
//  Used by:
//  - game.cpp (main game)
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
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__world__
#define __Bawk__world__

#include <stdio.h>
#include <vector>
#include "basic_types.h"
#include "worldrender.h"

class World {
    
    // how many cycles the world has lived through
    unsigned long age;
    
    // vector of objects
    std::vector<SuperObject*> superobjects;
    
public:
    World();
    ~World();
    int load_resources();
    void free_resources();
    void render(fmat4* transform);
    void update_chunks(fvec3* old_pos, fvec3* new_pos);
    void step();
};

#endif /* defined(__Bawk__world__) */
