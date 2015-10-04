//
//  world.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "world.h"
#include "base_world.h"
#include "block_loader.h"

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

// cycles one timestep for the world
void World::step() {
    age++;
}