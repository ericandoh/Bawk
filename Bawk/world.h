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
#include <string>
#include "basic_types.h"
#include "worldrender.h"

class World {
    
    // how many cycles the world has lived through
    unsigned long age;
    
    std::string name;
    
    // vector of objects
    std::vector<SuperObject*> superobjects;
    
    SuperObject* base_world;
    
public:
    World(std::string id);
    ~World();
    int load_resources();
    void free_resources();
    void render(fmat4* transform);
    void update_chunks(fvec3* old_pos, fvec3* new_pos);
    // called by cursor item, places block into baseworld
    void place_block(ivec3 position, block_type block);
    block_type get_block(float x, float y, float z);
    // called by game to kill a block directly. then effects are propogated to the appropriate entities
    bool kill_block(ivec3* src);
    void step();
    SuperObject* make_bounded_super_object();
    bool will_collide_with_anything(RenderableSuperObject* superobject);
};

#endif /* defined(__Bawk__world__) */
