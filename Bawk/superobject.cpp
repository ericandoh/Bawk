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
    name = "";
    world_name = wid;
    make_object_in_world_folder(wid, name);
}

SuperObject::SuperObject(std::string id, std::string wid) {
    name = id;
    world_name = wid;
    make_object_in_world_folder(wid, name);
}

int SuperObject::get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    return load_chunk_from_file(world_name, name, &pos, to_arr);
}

int SuperObject::save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    return save_chunk_to_file(world_name, name, &pos, from_arr);
}