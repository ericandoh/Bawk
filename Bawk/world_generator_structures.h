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

struct event_layer_info {
    block_layer_info layer;
    std::string name;
    std::unordered_map<std::string, float> event_parameters;
    event_layer_info() {
        layer = block_layer_info();
    }
};

struct biome_game_info {
    std::string name;
    float strength;
    float persistence;
    std::vector<block_layer_info> blocks;
    std::vector<block_layer_info> structures;
    std::vector<event_layer_info> events;
    biome_game_info() {
        name = "empty";
        strength = 0;
        persistence = 0;
    }
};


#endif
