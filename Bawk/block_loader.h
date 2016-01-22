//
//  Loads and saves chunk data (blocks)
//  -
//  -
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 10/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__block_loader__
#define __Bawk__block_loader__

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "basic_types.h"

// set these to toggle under which name things get saved
void set_current_world_name(std::string world_name);

// where the world will be stored
std::string get_path_to_world_folder();
std::string get_path_to_world();

// stored under player who owns, or if owned by world, under world's player folder
std::string get_path_to_superobj_folder(uint32_t pid, uint32_t vid);
std::string get_path_to_superobj(uint32_t pid, uint32_t vid);
std::string get_path_to_superobj_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos);

// stored under player
std::string get_path_to_template_folder(uint32_t pid, uint32_t vid);
std::string get_path_to_template(uint32_t pid, uint32_t vid);
std::string get_path_to_template_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos);

// stores the player
std::string get_path_to_player_folder(uint32_t pid);
std::string get_path_to_player(uint32_t pid);

// all the game data
std::string get_path_to_game_folder();
std::string get_path_to_game();
std::string get_path_to_game_json();

void delete_at_path(std::string path);

int validate_read_write_path(std::string path);

class IODataObject {
    std::ifstream infile;
    std::ofstream outfile;
    std::string path;
    SaveMode mode;
    unsigned long i;
    bool succeeded;
    char* read_data;
    long size;
public:
    IODataObject(std::string p);
    ~IODataObject();
    
    void close();
    
    template <typename T> void save_value(T v) {
        outfile.seekp(i);
        outfile.write((char*)&v, sizeof(T));
        i += sizeof(T);
    }
    
    template <typename T> void save_pointer(T* v, size_t s) {
        outfile.seekp(i);
        outfile.write((char*)v, s);
        i += s;
    }
    
    template <typename T> T read_value() {
        if (i + sizeof(T) > size)
            return T();
        T v;
        memcpy(&v, &(read_data[i]), sizeof(T));
        i += sizeof(T);
        return v;
    }
    
    template <typename T> int read_pointer(T* v, size_t s) {
        if (i + s > size)
            return 1;
        memcpy(v, &(read_data[i]), s);
        i += s;
        return 0;
    }
    
    // read/write methods
    int save(bool needs_validation=true);
    int read(bool needs_validation=true);
};

#endif /* defined(__Bawk__block_loader__) */
