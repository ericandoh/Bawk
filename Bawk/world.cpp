//
//  world.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "world.h"
#include "base_world.h"
#include "block_loader.h"
#include "superobject.h"
#include "blocktracer.h"
#include "game_info_loader.h"

// constructor
World::World(std::string id) {
    printf("Adding World\n");
    age = 0;
    name = id;
    set_current_world_name(id);
    // try loading itself
    load_self();
    base_world = new BaseWorld();
}

int World::load_self() {
    IODataObject reader(get_path_to_world());
    if (reader.read()) {
        return 1;
    }
    age = reader.read_value<unsigned long>();
    base_world.load_self(&reader);
    reader.close();
    return 0;
}

void World::remove_self() {
    IODataObject writer(get_path_to_world());
    if (writer.save()) {
        return;
    }
    writer.save_value(age);
    base_world.remove_self(&writer);
    writer.close();
}

// renders the world
void World::render(fmat4* transform) {
    //shade_intensity -= (abs((int)(age % 400) - 200)/200.0f) * 0.5f;
    //if (shade_intensity < 0.2f)
    //    shade_intensity = 0.2f;
    //set_shader_intensity(shade_intensity);
    base_world.render(transform);
}

void World::update_chunks(fvec3* player_pos) {
    base_world.update_chunks(player_pos);
}

void World::get_entity_at(float x, float y, float z, Entity** selected) {
    *selected = base_world.poke(x, y, z);
}

bool World::break_block() {
    Entity* src = get_look_at();
    if (!src)
        return false;
    
    ivec4 looking_at;
    if (!get_look_at(&looking_at)) {
        return false;
    }
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    
    printf("Removing at (%d, %d, %d)\n",
           mx, my, mz);
    
    src->break_block(mx, my, mz);
    return true;
}

/*
bool World::kill_block(ivec3* src) {
    ivec4 looking_at;
    if (!get_look_at(&looking_at)) {
        return false;
    }
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    
    Entity* selected;
    get_entity_at(mx, my, mz, &selected);
    
    block_type blk = get_block(mx, my, mz);
    if (!blk.type)
        return false;
    
    base_world->set_block(mx, my, mz, block_type());
    
    printf("Removing at (%d, %d, %d)   (face %d)\n",
           mx, my, mz, face);
    src->x = mx;
    src->y = my;
    src->z = mz;
    return true;
}*/

void World::add_player(Player* player) {
    base_world.add_entity(player);
}

void World::add_entity(Entity* entity) {
    base_world.add_entity(entity);
}

void World::add_event(WorldEvent *event) {
    for (int i = (int)events.size() - 1; i >= 0; i--) {
        if (events[i]->expiration <= event->expiration) {
            // insert at this index
            events.insert(events.begin() + i, event);
            return;
        }
    }
    events.insert(events.begin(), event);
}

// cycles one timestep for the world
void World::step() {
    base_world.step();
    
    int counter = 0;
    while (counter < events.size()) {
        if (events[counter]->expiration > age) {
            break;
        }
        events[counter]->action(this);
        if (events[counter]->expires) {
            free_world_event(events[counter]);
            events.erase(events.begin() + counter);
        }
        else {
            counter++;
        }
    }
    age++;
}

bool World::will_collide_with_anything(Entity* other) {
    return base_world.collides_with(other);
}

SuperObject* World::create_superobject(Player* player) {
    SuperObject* obj;
    if (player)
        obj = new SuperObject(player->getID(), player->assignID());
    else
        obj = new SuperObject(0, player->assignID());
    everything.add_entity(obj);
    return obj;
}

void World::remove_entity(Entity* entity) {
    everything.remove_entity(entity);
}