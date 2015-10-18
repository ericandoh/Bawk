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

enum SaveMode {
    READ, WRITE,
};

class IODataObject {
    std::ifstream infile;
    std::ofstream outfile;
    SaveMode mode;
    unsigned long i;
    bool succeeded;
    char* read_data;
    long size;
public:
    IODataObject();
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
            return 0;
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
    int save(std::string path);
    int read(std::string path);
    
    int save_to_world(std::string world_name);
    int read_from_world(std::string world_name);
    
    int save_to_world_chunk(std::string world_name, ivec3* chunk_pos);
    int read_from_world_chunk(std::string world_name, ivec3* chunk_pos);
    
    int save_to_superobj(uint32_t pid, uint32_t vid);
    int read_from_superobj(uint32_t pid, uint32_t vid);
    
    int save_to_superobj_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos);
    int read_from_superobj_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos);
    
    int save_to_template(uint32_t pid, uint32_t vid);
    int read_from_template(uint32_t pid, uint32_t vid);
    
    int save_to_template_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos);
    int read_from_template_chunk(uint32_t pid, uint32_t vid, ivec3* chunk_pos);
    
    int save_to_player(uint32_t pid);
    int read_from_player(uint32_t pid);
    
    int save_to_game();
    int read_from_game();
};

#endif /* defined(__Bawk__block_loader__) */
