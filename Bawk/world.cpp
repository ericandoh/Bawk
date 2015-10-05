//
//  world.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "world.h"
#include "base_world.h"
#include "block_loader.h"
#include "worldrender.h"    // used for getting which block to place

// constructor
World::World(std::string id) {
    printf("Adding World\n");
    age = 0;
    
    name = id;
    make_world_folder(name);
    
    BaseWorld* world = new BaseWorld(name);
    superobjects.push_back(world);
}

World::~World() {
    for (int i = 0; i < superobjects.size(); i++) {
        superobjects[i]->remove_self();
        delete superobjects[i];
    }
}

int World::load_resources() {
    return world_load_resources();
}

void World::free_resources() {
    world_free_resources();
}

// renders the world
void World::render(fmat4* transform) {
    for (int i = 0; i < superobjects.size(); i++) {
        superobjects[i]->render(transform);
    }
}

void World::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    for (int i = 0; i < superobjects.size(); i++) {
        superobjects[i]->update_chunks(old_pos, new_pos);
    }
}

void World::place_block() {
    ivec4 looking_at = get_look_at();
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    if(face == 0)
        mx++;
    if(face == 3)
        mx--;
    if(face == 1)
        my++;
    if(face == 4)
        my--;
    if(face == 2)
        mz++;
    if(face == 5)
        mz--;
    superobjects.front()->set_block(mx, my, mz, 15);
    printf("Placing at (%d, %d, %d)   (face %d)\n",
           mx, my, mz, face);
}

void World::kill_block() {
    ivec4 looking_at = get_look_at();
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    superobjects.front()->set_block(mx, my, mz, 0);
    printf("Removing at (%d, %d, %d)   (face %d)\n",
           mx, my, mz, face);
}

// cycles one timestep for the world
void World::step() {
    age++;
}