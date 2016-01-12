//
//  base_world.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "base_world.h"
#include "block_loader.h"
#include "world_generator.h"
#include <vector>
#include <unordered_map>

#define GROUND_DEPTH 4

BaseWorld::BaseWorld() {
    pid = 0;
    vid = 0;
    entity_class = EntityType::BASEWORLD;
    get_empty_chunk(air);
    get_empty_chunk(ground);
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            for (int y = 0; y < GROUND_DEPTH; y++) {
                ground[x][y][z].type = 2;
            }
        }
    }
    bounds = bounding_box(fvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX), fvec3(FLT_MAX, FLT_MAX, FLT_MAX));
}

// --- SuperObject ---
int BaseWorld::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    if (!SuperObject::get_chunk(to_arr, x, y, z)) {
        return 0;
    }
    fill_chunk_at(ivec3(x, y, z), to_arr);
    return 0;
}

bool BaseWorld::within_dimensions_chunk(int x, int y, int z) {
    // all the world is within dimensions technically
    return true;
}

void BaseWorld::update_dimensions_from_chunk(ivec3 chunk_pos) {
    // do nothing
}

bool BaseWorld::intersects_chunk(ivec3 lower, ivec3 upper, ivec3 chunkpos) {
    if (load_chunk(chunkpos.x, chunkpos.y, chunkpos.z) ) {
        return false;
    }
    bool result = chunks[chunkpos]->intersects_my_bounds(lower, upper);
    return result;
}

// --- Entity ---

void BaseWorld::step(Game* game, int ms) {
    SuperObject::step(game, ms);
}

bool BaseWorld::collides_with(Entity* other) {
    if (!can_collide || !other->can_collide) {
        return false;
    }
    
    bounding_box other_rwc_bounds;
    other->transform_into_world_coordinates(&other_rwc_bounds.lower, other->bounds.lower.x, other->bounds.lower.y, other->bounds.lower.z);
    other->transform_into_world_coordinates(&other_rwc_bounds.upper, other->bounds.upper.x, other->bounds.upper.y, other->bounds.upper.z);
    other_rwc_bounds.refit_for_rotation();
    
    int my_collision_level = get_collision_level() - 1;
    int other_collision_level = other->get_collision_level();
    
    // TOFU this is a hack, find a better way to deal
    if (other->entity_class == EntityType::SUPEROBJECT ||
        other->entity_class == EntityType::CURSORSUPEROBJECT ||
        other->entity_class == EntityType::GAMETEMPLATE) {
        if (SuperObject::collides_with(other, &other_rwc_bounds, &other_rwc_bounds, my_collision_level, other_collision_level - 1)) {
            return true;
        }
        else {
            for (Entity* ent: ((SuperObject*)other)->entities) {
                if (this->collides_with(ent)) {
                    return true;
                }
            }
        }
        return false;
    }
    else {
        return SuperObject::collides_with(other, &other_rwc_bounds, &other_rwc_bounds, my_collision_level, other_collision_level);
    }
}

