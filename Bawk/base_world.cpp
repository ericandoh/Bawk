//
//  base_world.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "base_world.h"
#include "block_loader.h"

#define GROUND_DEPTH 4

BaseWorld::BaseWorld(std::string wid) {
    pid = 0;
    vid = 0;
    world_name = wid;
    entity_class = 1;
    get_empty_chunk(air);
    get_empty_chunk(ground);
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            for (int y = 0; y < GROUND_DEPTH; y++) {
                ground[x][y][z].type = 2;
            }
        }
    }
}

std::string BaseWorld::get_save_path() {
    // don't save this world
    return "";
}

std::string BaseWorld::get_chunk_save_path(ivec3* pos) {
    return get_path_to_world_chunk(world_name, pos);
}

int BaseWorld::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    if (!SuperObject::get_chunk(to_arr, x, y, z)) {
        return 0;
    }
    // chunk doesn't exist in disk, so it must not exist at all
    // create a new chunk with random parameters
    if (y == 0) {
        //printf("Loading ground\n");
        memcpy(&to_arr[0][0][0], &ground[0][0][0], sizeof(block_type)*CX*CY*CZ);
        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 10 <=1)
                    to_arr[x][GROUND_DEPTH][z].type = rand() % 4 + 2;
            }
        }
    }
    else {
        //printf("Loading air\n");
        memcpy(&to_arr[0][0][0], &air[0][0][0], sizeof(block_type)*CX*CY*CZ);
        for (int x = 0; x < CX; x++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 100 <=1)
                    to_arr[x][rand() % 16][z].type = rand() % 4 + 7;
            }
        }
    }
    return 0;
}

bool BaseWorld::within_dimensions_chunk(int x, int y, int z) {
    // all the world is within dimensions technically
    return true;
}

void BaseWorld::update_dimensions_from_chunk(ivec3 chunk_pos) {
    // do nothing
}

bool BaseWorld::intersects_with_my_bounds(fvec3 lower_corner, fvec3 upper_corner) {
    return true;
}

bool BaseWorld::poke(float x, float y, float z) {
    return get_block(x, y, z).type;
}

int BaseWorld::get_collision_priority() {
    return 2;
}

void BaseWorld::remove_selfs() {
    save_all_chunks();
}
