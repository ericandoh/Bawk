//
//  biomegenerator.h
//  Bawk
//
//  Created by Eric Oh on 1/22/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__biomegenerator__
#define __Bawk__biomegenerator__

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include "basic_types.h"

class SectorGenerationInfo;
class TerrainGenerator;
class CursorSuperObject;

struct block_layer_info {
    uint16_t type;
    int lower, upper;
    float frequency;
    block_layer_info() {
        type = 0;
        lower = INT_MIN;
        upper = INT_MAX;
        frequency = 0;
    }
};

struct BiomeGenerationInfo {
    // in RWC, aligned to sector (so 0,0 to (width, height) of sector)
    ivec3 position;
    // biome id
    int bid;
    int unique_id;
    int_bounding_box range;
    TerrainGenerator* generator;
    BiomeGenerationInfo() {
        position = ivec3(0,0,0);
        bid = 0;
        unique_id = -1;
        range = int_bounding_box();
        generator = 0;
    }
    BiomeGenerationInfo(ivec3 p) {
        position = p;
        bid = 0;
        unique_id = -1;
        range = int_bounding_box();
        generator = 0;
    }
};

class BiomeGenerator {
public:
    uint16_t biome_id;
    std::string name;
    float strength;
    float persistence;
    std::vector<block_layer_info> blocks;
    std::vector<block_layer_info> structures;
    std::vector<TerrainGenerator*> valid_generators;
    //std::vector<event_layer_info> events;
    
    std::map<int, CursorSuperObject*> structure_models;
    
    BiomeGenerator();
    TerrainGenerator* get_random_generator();
    uint16_t get_random_block(int depth);
    uint16_t get_random_struct(int* depth);
    virtual void add_structures(SectorGenerationInfo* sector_info,
                                BiomeGenerationInfo* biome_info);
    
    void reset();
};


#endif /* defined(__Bawk__biomegenerator__) */
