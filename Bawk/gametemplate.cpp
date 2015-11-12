//
//  temporarytemplate.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "gametemplate.h"
#include "world.h"
#include "cursorsuperobject.h"

GameTemplate::GameTemplate() {
    pos = fvec3(0, 0, 0);   // unlike any other objects, this is alwas 0-aligned then
    entity_class = 5;
}

int GameTemplate::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    get_empty_chunk(to_arr);
    return 0;
}

CursorSuperObject* GameTemplate::create_from_template(Player* player, World* world, TemporaryTemplate* temp) {
    printf("Publishing template!\n");
    // package our blocks into a cursorsuperobject
    // TODO do not call this if we have no blocks here
    CursorSuperObject* object = new CursorSuperObject(player->getID(),
                                                      player->assignID());// all templates are made on the bar
    object->pos = bounds.lower;
    if (set_blocks(player, world, object)) {
        // save the object to disk
        // this doesn't actually delete the object from memory
        object->remove_selfs();
        return object;
    }
    delete object;
    return 0;
}

void GameTemplate::publish(Player* player, World* world) {
    if (PlaceableSuperObject::set_blocks(player, world, world->base_world)) {
        world->remove_entity(this);
    }
}

std::vector<block_data> TemporaryTemplate::publish(Player* player, World* world) {
    // first, remove myself from the world (without deleting myself from disk)
    // because like, im totally not in the disk brah
    world->remove_entity(this, false);
    if (will_be_independent) {
        
        // find the translation/other properties of this object
        ivec3 lower_corner(INT_MAX, INT_MAX, INT_MAX);
        for (auto &i : blocks) {
            ivec3 pos = i.position;
            lower_corner.x = std::min(lower_corner.x, pos.x);
            lower_corner.y = std::min(lower_corner.y, pos.y);
            lower_corner.z = std::min(lower_corner.z, pos.z);
        }
        
        // now package it into a superobject
        SuperObject* superobject = world->create_superobject(player, lower_corner);
        // and add the blocks in this template to the superobject. There should be no conflicts here
        
        for (auto &i : blocks) {
            ivec3 pos = i.position;
            superobject->set_block(pos.x, pos.y, pos.z, i.block);
        }
    }
    else {
        // publish all my blocks to the world!
        for (auto &i : blocks) {
            world->place_block(i.position, i.block);
        }
    }
    return blocks;
}

void TemporaryTemplate::unpublish(World* world) {
    world->remove_entity(this, false);
}

void TemporaryTemplate::render(fmat4* transform) {
    set_shader_intensity(1.0f);
    RenderableSuperObject::render(transform);
}

void TemporaryTemplate::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    // do nothing
}

int TemporaryTemplate::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    get_empty_chunk(to_arr);
    return 0;
}

int TemporaryTemplate::save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) {
    // do nothing
    return 0;
}