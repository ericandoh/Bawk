//
//  world_generator_structures.h
//  Bawk
//
//  Created by Eric Oh on 10/31/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_world_generator_structures_h
#define Bawk_world_generator_structures_h

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <climits>

struct world_gen_mode_info {
    std::string name;
    std::map<int, float> biome_frequencies;
    int xsectorsize, zsectorsize;
    int biomepointavg, biomepointvar;
    int islandpointavg, islandpointvar;
    float separation;
    float fatness;
    float melt_distance;
    int octaves;
    float lower_strength, lower_persistence;
    world_gen_mode_info() {
        name = "default";
        xsectorsize = zsectorsize = 256;
        biomepointavg = 7;
        biomepointvar = islandpointvar = 1;
        islandpointavg = 12;
        separation = 14.0f;
        fatness = 4.0f;
        melt_distance = 10.0f;
        octaves = 5;
        lower_strength = 10.0f;
        lower_persistence = 0.5f;
    }
};


#endif
