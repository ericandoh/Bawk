//
//  biomegenerator.cpp
//  Bawk
//
//  Created by Eric Oh on 1/22/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "biomegenerator.h"
#include "basic_types.h"
#include "sectorgenerator.h"

BiomeGenerator::BiomeGenerator() {
    name = "empty";
    strength = 0;
    persistence = 0;
}

uint16_t BiomeGenerator::get_random_block(int depth) {
    float total_frequency = 0;
    for (auto &i: blocks) {
        if (depth >= i.lower && depth <= i.upper) {
            total_frequency += i.frequency;
        }
    }
    
    float rv = choose_random_one();
    float total = 0;
    for (auto &i: blocks) {
        if (depth >= i.lower && depth <= i.upper) {
            total += i.frequency;
            if (rv < total / total_frequency) {
                return i.type;
            }
        }
    }
    return 0;
}

uint16_t BiomeGenerator::get_random_struct(int* depth) {
    float total_frequency = 0;
    for (auto &i: structures) {
        total_frequency += i.frequency;
    }
    float rv = choose_random_one() * total_frequency;
    float total = 0;
    for (auto &i: blocks) {
        total += i.frequency;
        if (rv < total) {
            // choose a depth between lower and upper
            rv = choose_random_one() * (i.upper - i.lower);
            *depth = (int)floorf(rv + i.lower);
            return i.type;
        }
    }
    return 0;
}

void BiomeGenerator::add_structures(SectorGenerationInfo* sector_info,
                                    BiomeGenerationInfo* biome_info) {
    int_bounding_box box = biome_info->range;
    int depth;
    for (int x = box.lower.x; x < box.upper.x; x++) {
        for (int z = box.lower.z; z < box.upper.z; z++) {
            uint16_t struct_id = get_random_struct(&depth);
            if (struct_id > 0) {
                // add struct at depth
                //sector_info->set_block(ivec3 pos, block_type blk);
            }
        }
    }
    
    // find # of structures to add depending on the bounding box
    // pick points that won't overlap
    // add structures
    
    
}
    