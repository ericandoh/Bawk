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
#include "superobject.h"

// constructor
World::World(std::string id) {
    printf("Adding World\n");
    age = 0;
    
    name = id;
    make_world_folder(name);
    
    base_world = new BaseWorld(name);
    superobjects.push_back(base_world);
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

void World::place_block(ivec3 position, uint16_t block) {
    // TODO (check for colision with other superobjects...?)
    // (other superobjects including baseworld!)
    /*
    // TODO
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
    mz--;*/
    base_world->set_block(position.x, position.y, position.z, block);
}

ivec3 World::kill_block() {
    ivec4 looking_at = get_look_at();
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    // TODO do a check to see if any objects in the way
    // TODO do a distance check
    base_world->set_block(mx, my, mz, 0);
    printf("Removing at (%d, %d, %d)   (face %d)\n",
           mx, my, mz, face);
    return ivec3(mx, my, mz);
}

// cycles one timestep for the world
void World::step() {
    age++;
}

SuperObject* World::make_bounded_super_object() {
    return new SuperObject("item", name);
}

bool World::will_collide_with_anything(RenderableSuperObject* other) {
    for (int i = 0; i < superobjects.size(); i++) {
        if (superobjects[i] == other) {
            continue;
        }
        if (superobjects[i]->collides_with(other)) {
            return true;
        }
    }
    return false;
}