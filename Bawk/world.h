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
#include "entityholder.h"

class World {
    
    // how many cycles the world has lived through
    unsigned long age;
    
    SuperObject* base_world;
    EntityHolder holder;
public:
    std::string name;
    World(std::string id);
    int load_self();
    void remove_self();
    void render(fmat4* transform);
    void update_chunks(fvec3* old_pos, fvec3* new_pos);
    // called by cursor item, places block into baseworld
    void place_block(ivec3 position, block_type block);
    block_type get_block(float x, float y, float z);
    void get_at(float x, float y, float z, bool* world_selected, Entity** selected);
    // called by game to kill a block directly. then effects are propogated to the appropriate entities
    bool kill_block(ivec3* src);
    void add_player(Player* player);
    void step();
    bool will_collide_with_anything(RenderableSuperObject* superobject);
    SuperObject* create_superobject(Player* player);
};

#endif /* defined(__Bawk__world__) */
