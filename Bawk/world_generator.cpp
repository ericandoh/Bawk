//
//  world_generator.cpp
//  Bawk
//
//  Created by Eric Oh on 10/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "world_generator.h"
#include <unordered_map>
#include <vector>
#include <glm/gtc/noise.hpp>

namespace std
{
    template<>
    struct hash<ivec3> {
        size_t operator()(const ivec3 &pt) const {
            // turn first 9 bits to a 27 bit string, then add together
            // the result should be XYZXYZXYZXYZ for XXXX, YYYY, ZZZZ (but with more bits)
            uint32_t xb = spread_bits(pt.x);
            uint32_t yb = spread_bits(pt.y) << 1;
            uint32_t zb = spread_bits(pt.z) << 2;
            return xb + yb + zb;
        }
    };
}

// holds info about the world generation
struct WorldGeneratorInfo {
    // sizes of the sectors where islands spawn (RWC)
    int xsector_size;
    int ysector_size;
    int zsector_size;
    
    // the below - not exactly variance, but close enough...
    // number of possible biome points to pick per sector
    int biome_points_per_sector_average;
    int biome_points_per_sector_variance;
    
    // minimum distance between biome points (RWC)
    float min_biome_point_separation;
    
    // number of biome points to put into an island
    int biome_points_per_island_average;
    int biome_points_per_island_variance;
    
    /*
     Seeds
     Most turn out to be weeds
     But a few dream
     And expand the seam
     Of our reality
     */
    int seed;
};

// holds state of the world generation, for easier generation
struct WorldGeneratorState {
    // biome points per sector (sector pos (QPC) TO position within sector(RWC))
    std::unordered_map<ivec3, std::vector<ivec3>> biome_points;
    // the island points picked
    std::unordered_map<ivec3, std::vector<ivec3>> island_points;
    std::unordered_map<ivec3, std::vector<int>> island_biome_points;
};

WorldGeneratorInfo info;
WorldGeneratorState state;

void setup_world_generator(int fillthis) {
    // later pass in arguments from above
    info.xsector_size = 1600;
    info.ysector_size = 1600;
    info.zsector_size = 1600;
    
    info.biome_points_per_sector_average = 20;
    info.biome_points_per_sector_variance = 7;
    
    info.min_biome_point_separation = 200;
    
    info.biome_points_per_island_average = 4;
    info.biome_points_per_island_variance = 3;
    
    info.seed = fillthis;
}

int choose_random(int average, int variance) {
    int result = average;
    for (int i = 0; i < variance; i++) {
        if (rand() % 2 == 0) {
            result++;
        }
        else {
            result--;
        }
    }
    return result;
}

float get_distance(ivec3 a, ivec3 b) {
    // fuck glm and its fucking
    return sqrtf( (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z) );
}

void generate_biome_points(ivec3 sector_pos) {
    ivec3 sector_pos_offset(sector_pos.x * info.xsector_size,
                            sector_pos.y * info.ysector_size,
                            sector_pos.z * info.zsector_size);
    
    srand(info.seed);
    srand(rand() + sector_pos.x);
    srand(rand() + sector_pos.y);
    srand(rand() + sector_pos.z);
    
    std::vector<ivec3> biome_points;
    int biome_point_count = choose_random(info.biome_points_per_sector_average, info.biome_points_per_sector_variance);
    biome_points.reserve(biome_point_count);
    int biome_point_counter = 0;
    // try 7 times to find a point more than info.min_biome_point_separation away
    int max_attempts = 7;
    
    bool sector_not_filled = true;
    while (sector_not_filled && biome_point_counter < biome_point_count) {
        if (biome_points.empty()) {
            biome_points.push_back(ivec3(rand() % info.xsector_size + sector_pos_offset.x,
                                         rand() % info.ysector_size + sector_pos_offset.y,
                                         rand() % info.zsector_size + sector_pos_offset.z));
        }
        else {
            bool success = true;
            for (int attempts = 0; attempts < max_attempts; attempts++) {
                ivec3 point = ivec3(rand() % info.xsector_size + sector_pos_offset.x,
                                    rand() % info.ysector_size + sector_pos_offset.y,
                                    rand() % info.zsector_size + sector_pos_offset.z);
                success = true;
                for (auto &i: biome_points) {
                    if (get_distance(i, point) < info.min_biome_point_separation) {
                        success = false;
                    }
                }
                if (success) {
                    biome_points.push_back(point);
                    break;
                }
            }
            if (!success) {
                // all my attempts at adding biome points failed miserably
                // le miserable sad :(
                sector_not_filled = false;
            }
        }
        
        biome_point_counter++;
    }
    state.biome_points[sector_pos] = biome_points;
}

void generate_island_points(ivec3 sector_pos) {
    if (!state.biome_points.count(sector_pos)) {
        generate_biome_points(sector_pos);
    }
    /*for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = -1; k <= 1; k++) {
                ivec3 side_sector_pos(sector_pos.x + i, sector_pos.y + j, sector_pos.z + k);
                if (!state.biome_points.count(side_sector_pos)) {
                    generate_biome_points(side_sector_pos);
                }
            }
        }
    }*/
    
    srand(info.seed);
    srand(rand() + sector_pos.x);
    srand(rand() + sector_pos.y);
    srand(rand() + sector_pos.z);
    srand(rand());  // call rand one more time to distinguish from generate_biome_points
    
    std::vector<ivec3> island_points;
    int sector_biome_point_count = (int) state.biome_points[sector_pos].size();
    
    int island_point_count = choose_random(info.biome_points_per_island_average, info.biome_points_per_island_variance);
    if (island_point_count > sector_biome_point_count)
        island_point_count = sector_biome_point_count;
    
    island_points.reserve(island_point_count);
    
    std::vector<int> indices = std::vector<int>(sector_biome_point_count);
    for (int i = 0; i < sector_biome_point_count; i++) {
        indices[i] = i;
    }
    
    for (int i = 0; i < island_point_count; i++) {
        int index_size = (int)indices.size();
        int index_of_index = 0;
        if (island_points.empty()) {
            // pick a random point to start with
            index_of_index = rand() % index_size;
        }
        else {
            // pick the closest index
        }
        
        
        
        int use_index = indices[index_of_index];
        island_points.push_back(state.biome_points[sector_pos][use_index]);
        indices[index_of_index] = indices.back();
        indices.pop_back();
    }
    state.island_points[sector_pos] = island_points;
}

static float noise2d(float x, float y, int seed, int octaves, float persistence, float strength) {
    srand(seed);
    // any more and we start losing precision
    // 2^20 is about a million, so about a million x million possible worlds. I think we good
    int add_x = rand() % (0x1 << 20);
    int add_y = rand() % (0x1 << 20);
    float sea_level = 5.0;
    
    float sum = 0;
    float scale = 1.0;
    
    for(int i = 0; i < octaves; i++) {
        sum += strength * glm::simplex(fvec2((x+add_x) / 256.0, (y+add_y) / 256.0) * scale);
        scale *= 2.0;
        strength *= persistence;
    }
    return sum + sea_level;
}

void fill_chunk_at(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]) {
    
    get_empty_chunk(to_arr);
    
    if (chunk_pos.y != -1) {
        return;
    }
    info.seed = 100;
    
    int octaves = 5; //how many frequencies to have, let's not change this
    float persistence = 0.7;    // how much of the higher frequencies to have?
    float strength = 4.0;      // how much the terrain should change
    
    // copy me
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            int height = (int)floorf(noise2d(x + chunk_pos.x*CX, z + chunk_pos.z*CZ, info.seed, octaves, persistence, strength));
            height = abs(height);
            if (height > CY)
                height = CY;
            if (height < 2)
                height = 2;
            for (int y = 0; y < height; y++) {
                to_arr[x][y][z].type = 2;
            }
        }
    }
}

void fill_chunk_at_one_island(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]) {
    
}

void clean_world_generator() {
    // nothing needs to be done (yet)
}

