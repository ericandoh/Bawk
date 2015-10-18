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
    // try loading itself
    load_self();
    
    base_world = new BaseWorld(name);
    
    holder.set_global_entity(base_world);
}

int World::load_self() {
    IODataObject reader;
    if (reader.read_from_world(name)) {
        return 1;
    }
    age = reader.read_value<unsigned long>();
    reader.close();
    return 0;
}

void World::remove_self() {
    IODataObject writer;
    if (writer.save_to_world(name)) {
        return;
    }
    writer.save_value(age);
    writer.close();
}

// renders the world
void World::render(fmat4* transform) {
    holder.render(transform);
}

void World::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    holder.update_chunks(old_pos, new_pos);
}

void World::place_block(ivec3 position, block_type block) {
    base_world->set_block(position.x, position.y, position.z, block);
}

block_type World::get_block(float x, float y, float z) {
    return base_world->get_block(x, y, z);
}

bool World::kill_block(ivec3* src) {
    ivec4 looking_at;
    if (!get_look_at(&looking_at)) {
        return false;
    }
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    // TODO do a check to see if any objects in the way
    // TODO do a distance check
    base_world->set_block(mx, my, mz, 0);
    printf("Removing at (%d, %d, %d)   (face %d)\n",
           mx, my, mz, face);
    src->x = mx;
    src->y = my;
    src->z = mz;
    return true;
}

void World::add_player(Player* player) {
    holder.add_entity(player);
}

// cycles one timestep for the world
void World::step() {
    holder.step();
    age++;
}

bool World::will_collide_with_anything(RenderableSuperObject* other) {
    return holder.collides_with(other);
}

SuperObject* World::create_superobject(Player* player) {
    return new SuperObject(name, player->getID(), player->assignID());
}