//
//  sectorgenerator.h
//  Bawk
//
//  Created by Eric Oh on 1/14/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__sectorgenerator__
#define __Bawk__sectorgenerator__

#include <stdio.h>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "basic_types.h"
#include "block.h"

class SuperObject;
struct ChunkData;

struct biome_position_info {
    int lower, upper;
    int biomes[2];
    float biome_weights[2];
    int biome_unique_id;
    biome_position_info() {
        lower = upper = 0;
        biomes[0] = 0;
        biomes[1] = 0;
        biome_unique_id = 0;
        biome_weights[0] = 1.0f;
        biome_weights[1] = 0;
    }
};

struct SectorGenerationInfo {
    //std::unordered_map<ivec3, ChunkData*> sector_chunks;
    SuperObject* target;
    std::vector<std::vector<biome_position_info>> biome_map;
    SectorGenerationInfo(SuperObject* t);
    ~SectorGenerationInfo();
};

class SectorGenerator {
protected:
    virtual bool can_create_sector(ivec3 sector_pos);
    int_bounding_box transform_into_bounds(ivec3 sector_pos);
public:
    // --- PROPERTIES ---
    std::string name;
    std::map<int, float> biome_frequencies;
    
    // width/height of the sector, this should be CHUNK-ALIGNED!!! (else unknown stuff will happen)
    int sector_xwidth, sector_zwidth;
    // how much of the world should be land
    float fill_perc_avg, fill_perc_var;
    // how many biomes to make, roughly (excluding air biomes)
    int biome_count_avg, biome_count_var;
    // how many islands to have on average, the actual # of islands might differ
    int island_count_avg, island_count_var;
    
    float melt_distance;
    float fatness;
    
    float lower_strength, lower_persistence;
    int octaves;
    
    int seed;
    SectorGenerator();
    
    ivec3 transform_into_sector_pos(ivec3 chunk_pos);

    // called when a chunk doesn't exist in memory, see if we "can" create the sector needed for it
    bool try_create_sector(ivec3 chunk_pos, SuperObject* obj);
    
    void generate_sector(ivec3 sector_pos, SuperObject* obj);
};

void test_sector_generator();



#endif /* defined(__Bawk__sectorgenerator__) */
