//
//  block_loader.cpp
//  Bawk
//https://buddypanda.com/?p=10
//
//  Created by Eric Oh on 10/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "block_loader.h"
#include <iostream>
#include <fstream>

const std::string BLOCK_DATA_PATH = "/Users/Eric/w_data";
const std::string WORLD_SUBDIRECTORY_PATH = "world";

std::string get_chunk_path(std::string world_name,
                           std::string object_name,
                           ivec3* chunk_pos) {
    if (object_name.compare("") == 0) {
        object_name = WORLD_SUBDIRECTORY_PATH;
    }
    
    // pos for positive, not position!
    unsigned int xpos = chunk_pos->x > 0;
    unsigned int ypos = chunk_pos->y > 0;
    unsigned int zpos = chunk_pos->z > 0;
    
    unsigned int pos_bits = (xpos << 0x2) + (ypos << 0x1) + zpos;
    
    // for now put all chunk data in same object folder
    // later separate different x/y/z to different folders,
    // or hash locally (using same hash funct as for the maps)!
    std::string full_path = BLOCK_DATA_PATH + "/"
                            + world_name + "/"
                            + object_name + "/"
                            + "bl" + std::to_string(pos_bits) + "_"
                            + std::to_string(abs(chunk_pos->x)) + "_"
                            + std::to_string(abs(chunk_pos->y)) + "_"
                            + std::to_string(abs(chunk_pos->z));
    
    return full_path;
}

void make_world_folder(std::string world_name) {
    // this is unix specific. not sure if this works on windows...
    // later just import in boost and deal with it that way
    std::string command = "mkdir " + BLOCK_DATA_PATH + "/" + world_name;
    std::system(command.c_str());   // this might fail...TOFU handle
}

void make_object_in_world_folder(std::string world_name,
                                 std::string object_name) {
    if (object_name.compare("") == 0) {
        object_name = WORLD_SUBDIRECTORY_PATH;
    }
    // this is unix specific. not sure if this works on windows...
    // later just import in boost and deal with it that way
    std::string command = "mkdir " + BLOCK_DATA_PATH + "/" + world_name + "/" + object_name;
    std::system(command.c_str());   // this might fail...TOFU handle
}


int save_chunk_to_file(std::string world_name,
                std::string object_name,
                ivec3* chunk_pos,
                uint16_t block[CX][CY][CZ]) {
    std::ofstream out;
    out.open(get_chunk_path(world_name, object_name, chunk_pos),
             std::ios::out | std::ios::trunc | std::ios::binary);
    
    if (!out.is_open()) {
        printf("Failed to save chunk %d,%d,%d for object %s to file %s\n",
               chunk_pos->x, chunk_pos->y, chunk_pos->z, object_name.c_str(),
               get_chunk_path(world_name, object_name, chunk_pos).c_str());
        return -1;
    }
    
    // use any encoding/decoding function here
    // TOFU use run length encoding
    
    out.write((char*)&block[0][0][0], sizeof(block[0][0][0])*CX*CY*CZ);
    
    out.close();
    return 0;
}

int load_chunk_from_file(std::string world_name,
                std::string object_name,
                ivec3* chunk_pos,
                uint16_t block[CX][CY][CZ]) {
    std::ifstream in;
    in.open(get_chunk_path(world_name, object_name, chunk_pos),
            std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        // might try to read chunks that don't exist a lot of times, dont fret!
        //printf("Failed to load chunk %d,%d,%d for object %s\n",
        //       chunk_pos->x, chunk_pos->y, chunk_pos->z, object_name.c_str());
        return -1;
    }
    
    // use any encoding/decoding function here
    
    //in.seekg(0, std::ios::beg);
    in.read((char*)&block[0][0][0], sizeof(block[0][0][0])*CX*CY*CZ);
    
    in.close();
    return 0;
}