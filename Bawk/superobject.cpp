//
//  Superobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "superobject.h"
#include "block_loader.h"

SuperObject::SuperObject(std::string w) {
    // this constructor should be only called to construct a world
    vid = 0;
    pid = 0;
    world_name = w;
}

SuperObject::SuperObject(uint32_t p, uint32_t v) {
    // this constructor should be called for world-less, cursor objects
    vid = v;
    pid = p;
    world_name = "";
}

SuperObject::SuperObject(std::string wid, uint32_t p, uint32_t v, ivec3 po) {
    // this constructor should be called by a player wanting to make a new object
    // the player will assign the object an ID
    vid = v;
    pid = p;
    world_name = wid;
    pos = fvec3((float)po.x, (float)po.y, (float)po.z);
}

SuperObject::SuperObject(std::string wid, uint32_t p, uint32_t v, int* err) {
    // this constructor should be called if the object already exists and needs to be
    // loaded from memory
    vid = v;
    pid = p;
    world_name = wid;
    // call load_self() externally
    printf("frog\n");
}

std::string SuperObject::get_save_path() {
    return get_path_to_superobj(pid, vid);
}

std::string SuperObject::get_chunk_save_path(ivec3* pos) {
    return get_path_to_superobj_chunk(pid, vid, pos);
}

int SuperObject::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    IODataObject reader(get_chunk_save_path(&pos));
    if (reader.read(false))
        return 1;
    reader.read_pointer(&(to_arr[0][0][0]), sizeof(to_arr[0][0][0])*CX*CY*CZ);
    reader.close();
    return 0;
}

int SuperObject::save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    IODataObject writer(get_chunk_save_path(&pos));
    if (writer.save(false))
        return 1;
    writer.save_pointer(&(from_arr[0][0][0]), sizeof(from_arr[0][0][0])*CX*CY*CZ);
    writer.close();
    return 0;
}