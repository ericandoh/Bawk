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

SuperObject::SuperObject(std::string wid, uint32_t p, uint32_t v) {
    // this constructor should be called by a player wanting to make a new object
    // the player will assign the object an ID
    vid = v;
    pid = p;
    world_name = wid;
}

SuperObject::SuperObject(std::string wid, uint32_t p, uint32_t v, int* err) {
    // this constructor should be called if the object already exists and needs to be
    // loaded from memory
    vid = v;
    pid = p;
    world_name = wid;
    *err = load_self();
}

int SuperObject::load_self() {
    IODataObject reader;
    if (reader.read_from_superobj(pid, vid))
        return 1;
    // save chunk_bounds
    int chunk_count = reader.read_value<int>();
    for (int i = 0; i < chunk_count; i++) {
        ivec3 pos = reader.read_value<ivec3>();
        struct chunk_bounds bounds = reader.read_value<struct chunk_bounds>();
        chunk_bounds[pos] = bounds;
    }
    reader.close();
    return 0;
}

void SuperObject::remove_self() {
    RenderableSuperObject::remove_self();
    
    IODataObject writer;
    if (writer.save_to_superobj(pid, vid)) {
        printf("Failed to save superobject %d,%d\n", pid, vid);
        return;
    }
    int chunk_count = (int)chunk_bounds.size();
    writer.save_value(chunk_count);
    for (auto &i : chunk_bounds) {
        writer.save_value(i.first);
        writer.save_value(i.second);
    }
    writer.close();
}

int SuperObject::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    IODataObject reader;
    if (reader.read_from_superobj_chunk(pid, vid, &pos))
        return 1;
    reader.read_pointer(&(to_arr[0][0][0]), sizeof(to_arr[0][0][0])*CX*CY*CZ);
    reader.close();
    return 0;
}

int SuperObject::save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) {
    ivec3 pos = ivec3(x, y, z);
    IODataObject writer;
    if (writer.save_to_superobj_chunk(pid, vid, &pos))
        return 1;
    writer.save_pointer(&(from_arr[0][0][0]), sizeof(from_arr[0][0][0])*CX*CY*CZ);
    writer.close();
    return 0;
}

bool SuperObject::within_dimensions_chunk(int x, int y, int z) {
    //ivec3 pos = ivec3(x, y, z);
    printf("Optimize me by saving a list of the valid chunks xyz in a chunk data object");
    printf("Without it it still works for now but it's janky!");
    return true;
}