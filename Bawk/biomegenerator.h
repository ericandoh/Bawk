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
#include "basic_types.h"

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

class SectorGenerator;

class BiomeGenerator {
public:
    
    std::string name;
    float strength;
    float persistence;
    std::vector<block_layer_info> blocks;
    std::vector<block_layer_info> structures;
    //std::vector<event_layer_info> events;
    
    BiomeGenerator();
    
    uint16_t get_random_block(int depth);
    virtual void add_structures(SectorGenerator* parent, bounding_box range, uint16_t unique_id);
};


#endif /* defined(__Bawk__biomegenerator__) */
