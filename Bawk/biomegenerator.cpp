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
    
    int precision = 10000;
    float rv = (rand() % precision) * 1.0 / precision;
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

void BiomeGenerator::add_structures(SectorGenerationInfo* sector_info,
                                    BiomeGenerationInfo* biome_info) {
    // find # of structures to add depending on the bounding box
    // pick points that won't overlap
    // add structures
    
}
    