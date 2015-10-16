//
//  Superobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "superobject.h"
#include "block_loader.h"

SuperObject::SuperObject(std::string wid) {
    // this constructor should be only called to construct a world
    name = "";
    world_name = wid;
    make_object_in_world_folder(wid, name);
    // TODO load data into chunk_bounds
}

SuperObject::SuperObject(std::string id, std::string wid) {
    name = id;
    world_name = wid;
    make_object_in_world_folder(wid, name);
    // TODO load data into chunk_bounds
}

void SuperObject::remove_self() {
    RenderableSuperObject::remove_self();
    // TODO save data from chunk_bounds (or from the chunk directly) here
}

int SuperObject::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    return load_chunk_from_file(world_name, name, &pos, to_arr);
}

int SuperObject::save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    return save_chunk_to_file(world_name, name, &pos, from_arr);
}

bool SuperObject::within_dimensions_chunk(int x, int y, int z) {
    //ivec3 pos = ivec3(x, y, z);
    printf("Optimize me by saving a list of the valid chunks xyz in a chunk data object");
    printf("Without it it still works for now but it's janky!");
    return true;
}