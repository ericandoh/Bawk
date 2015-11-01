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
#include <map>
#include <glm/gtc/noise.hpp>
#include "game_info_loader.h"

/*
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
}*/

/*
// per biome, holds info about how to generate said biome
struct BiomeNoiseGenerationInfo {
    float strength_lower;
    float persistence_lower;
    float strength_upper;
    float persistence_upper;
    BiomeNoiseGenerationInfo();
    BiomeNoiseGenerationInfo(float s, float p);
    BiomeNoiseGenerationInfo(float sl, float pl, float su, float pu);
};

BiomeNoiseGenerationInfo::BiomeNoiseGenerationInfo() {
    strength_lower = persistence_lower = strength_upper = persistence_upper = 0.0f;
}

BiomeNoiseGenerationInfo::BiomeNoiseGenerationInfo(float s, float p) {
    strength_lower = 4.0f;
    persistence_lower = 0.3f;
    strength_upper = s;
    persistence_upper = p;
}
BiomeNoiseGenerationInfo::BiomeNoiseGenerationInfo(float sl, float pl, float su, float pu) {
    strength_lower = sl;
    persistence_lower = pl;
    strength_upper = su;
    persistence_upper = pu;
}

// holds info about the world generation
struct WorldGeneratorInfo {
    // sizes of the sectors where islands spawn (RWC)
    int xsector_size;
    int ysector_size;   // largely irrelevant
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
    
    // how fat the island is (top-down)
    // haha, fat
    float island_fatness;
    
    // how much to seam
    float melt_distance;
    
    int octaves;
    
    std::vector<BiomeNoiseGenerationInfo> biome_noise_info;
    
    ///
     Seeds
     Most turn out to be weeds
     But a few dream
     And expand the seam
     Of our reality
     //
    int seed;
};*/

// holds state of the world generation, for easier generation
struct WorldGeneratorState {
    // biome points per sector (sector pos (QPC) TO position within sector(RWC))
    std::unordered_map<ivec3, std::vector<ivec3>> biome_points;
    // what biome it corresponds to
    std::unordered_map<ivec3, std::vector<int>> island_biome_points;
    // the height of the island at that sector
    std::unordered_map<ivec3, int> island_heights;
};

world_gen_mode_info* info;
WorldGeneratorState state;
int seed;

void setup_world_generator(world_gen_mode_info* inf, int s) {
    info = inf;
    seed = s;
    /*
    // later pass in arguments from above
    int side = 256;  //1600
    info.xsector_size = side;
    info.ysector_size = side;
    info.zsector_size = side;
    
    info.biome_points_per_sector_average = 7;  //20
    info.biome_points_per_sector_variance = 1;  //7
    
    info.min_biome_point_separation = 14;    //200
    
    info.biome_points_per_island_average = 2;   //4
    info.biome_points_per_island_variance = 1;  //3
    
    info.island_fatness = 4.0f; // 7.0f
    info.melt_distance = 20.0f;  //20.0f
    
    info.octaves = 5;
    
    info.seed = fillthis;
    
    info.biome_noise_info.resize(5);
    info.biome_noise_info[0] = BiomeNoiseGenerationInfo(1.0f, 0.5f);//corresponds to the empty biome lol
    info.biome_noise_info[1] = BiomeNoiseGenerationInfo(20.0f, 0.2f);
    info.biome_noise_info[2] = BiomeNoiseGenerationInfo(20.0f, 0.7f);
    info.biome_noise_info[3] = BiomeNoiseGenerationInfo(20.0f, 0.2f);
    info.biome_noise_info[4] = BiomeNoiseGenerationInfo(20.0f, 0.7f);*/
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
    ivec3 sector_pos_offset(sector_pos.x * info->xsectorsize,
                            0,
                            sector_pos.z * info->zsectorsize);
    
    srand(seed);
    srand(rand() + sector_pos.x);
    //srand(rand() + sector_pos.y);
    srand(rand() + sector_pos.z);
    
    int biome_point_count = choose_random(info->biomepointavg, info->biomepointvar);
    state.biome_points[sector_pos].reserve(biome_point_count);
    state.island_biome_points[sector_pos].reserve(biome_point_count);
    
    int biome_point_counter = 0;
    // try 7 times to find a point more than info.min_biome_point_separation away
    int max_attempts = 7;
    
    bool sector_not_filled = true;
    while (sector_not_filled && biome_point_counter < biome_point_count) {
        if (state.biome_points[sector_pos].empty()) {
            state.biome_points[sector_pos].push_back(ivec3(rand() % info->xsectorsize + sector_pos_offset.x,
                                                           0, //rand() % info.ysector_size + sector_pos_offset.y,
                                                           rand() % info->zsectorsize + sector_pos_offset.z));
            state.island_biome_points[sector_pos].push_back(0);
        }
        else {
            bool success = true;
            for (int attempts = 0; attempts < max_attempts; attempts++) {
                ivec3 point = ivec3(rand() % info->xsectorsize + sector_pos_offset.x,
                                    0, //rand() % info.ysector_size + sector_pos_offset.y,
                                    rand() % info->zsectorsize + sector_pos_offset.z);
                success = true;
                for (auto &i: state.biome_points[sector_pos]) {
                    if (get_distance(i, point) < info->separation) {
                        success = false;
                        break;
                    }
                }
                if (success) {
                    state.biome_points[sector_pos].push_back(point);
                    state.island_biome_points[sector_pos].push_back(0);
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
}

int pick_biome_from_weights(std::map<int, float> &weights) {
    float total_total = 0;
    for (auto &i: weights) {
        total_total += i.second;
    }
    // probably a good idea to set the seed here, although we already have it set from above
    int precision = 10000;
    float rv = (rand() % precision) * 1.0 / precision;
    float total = 0;
    for (auto &i: weights) {
        total += i.second;
        if (rv < total / total_total)
        return i.first;
    }
    // this should never be executed if weights was setup correctly
    return 0;
}

void generate_island_points(ivec3 sector_pos) {
    generate_biome_points(sector_pos);
    
    int sector_biome_point_count = (int) state.biome_points[sector_pos].size();
    
    int island_point_count = choose_random(info->islandpointavg, info->islandpointvar);
    if (island_point_count > sector_biome_point_count)
        island_point_count = sector_biome_point_count;
    
    std::vector<int> indices = std::vector<int>(sector_biome_point_count);
    for (int i = 0; i < sector_biome_point_count; i++) {
        indices[i] = i;
    }
    ivec3 first_center;
    for (int i = 0; i < island_point_count; i++) {
        int index_size = (int)indices.size();
        int index_of_index = 0;
        if (i == 0) {
            // pick a random point to start with
            index_of_index = rand() % index_size;
        }
        else {
            // pick the closest index
            float shortest = info->xsectorsize + info->zsectorsize;
            index_of_index = 0;
            for (int j = 0; j < index_size; j++) {
                float dst = get_distance(first_center, state.biome_points[sector_pos][indices[j]]);
                if (dst < shortest) {
                    shortest = dst;
                    index_of_index = j;
                }
            }
        }
        int use_index = indices[index_of_index];
        state.island_biome_points[sector_pos][use_index] = pick_biome_from_weights(info->biome_frequencies);
        
        if (i == 0) {
            first_center = state.biome_points[sector_pos][use_index];
        }
        // this deletion is unnecessary - a simple .erase(...) would have sufficed...
        indices[index_of_index] = indices.back();
        indices.pop_back();
    }
    // set the relative height of this island
    state.island_heights[sector_pos] = rand() % 9 - 5;
    
    /*if (sector_pos.x == 0 && sector_pos.y == 0 && sector_pos.z == 0) {
        // force a small island of some sort at 0,0,0 for testing
        state.biome_points[sector_pos].push_back(ivec3(0, 0, 0));
        state.island_biome_points[sector_pos].push_back(7);
    }*/
    printf("Built sector %d %d %d\n", sector_pos.x, sector_pos.y, sector_pos.z);
}

static float noise2d(float x, float z, int seed, int octaves, float persistence, float strength) {
    srand(seed);
    // any more and we start losing precision
    // 2^20 is about a million, so about a million x million possible worlds. I think we good
    int add_x = rand() % (0x1 << 20);
    int add_z = rand() % (0x1 << 20);
    
    float sum = 0;
    float scale = 1.0;
    float total_strength = 0.0f;
    float base_strength = strength;
    
    for(int i = 0; i < octaves; i++) {
        sum += strength * glm::simplex(fvec2((x+add_x) / 256.0 * scale, (z+add_z) / 256.0 * scale));
        total_strength += strength;
        scale *= 2.0;
        strength *= persistence;
    }
    return std::abs(sum / total_strength * base_strength);
}

bool get_heights_at(int* lower, int* upper, ivec3 pos, std::map<int, float> &weights) {
    ivec3 sac;
    sac.x = (int)floorf(pos.x * 1.0 / info->xsectorsize);
    sac.y = 0;
    sac.z = (int)floorf(pos.z * 1.0 / info->zsectorsize);
    
    ivec3 src;
    src.x = ((pos.x % info->xsectorsize) + info->xsectorsize) % info->xsectorsize;
    src.y = 0;
    src.z = ((pos.z % info->zsectorsize) + info->zsectorsize) % info->zsectorsize;
    
    pos.y = 0;  // ignore the height component, bc fuck that shit
    
    int xlower, xupper, zlower, zupper;
    xlower = xupper = 0;
    zlower = zupper = 0;
    if (src.x < info->xsectorsize / 2)
        xlower = -1;
    else
        xupper = 1;
    if (src.z < info->zsectorsize / 2)
        zlower = -1;
    else
        zupper = 1;
    
    float melt_distance = info->melt_distance;
    std::vector<int> picked_biomes;
    std::vector<float> distances_to_said_biomes;
    for (int xc = sac.x + xlower; xc <= sac.x + xupper; xc++) {
        for (int zc = sac.z + zlower; zc <= sac.z + zupper; zc++) {
            ivec3 other_sac(xc, 0, zc);
            if (state.biome_points.count(other_sac) == 0) {
                generate_island_points(other_sac);
            }
            int points_in_this_sector = (int)state.biome_points[other_sac].size();
            for (int i = 0; i < points_in_this_sector; i++) {
                float distance = get_distance(pos, state.biome_points[other_sac][i]);
                if (distances_to_said_biomes.empty()) {
                    picked_biomes.push_back(state.island_biome_points[other_sac][i]);
                    distances_to_said_biomes.push_back(distance);
                }
                else if (distance <= distances_to_said_biomes[0] + melt_distance) {
                    // insert this pair into the list
                    int counter = 0;
                    bool inserted = false;
                    while (counter < distances_to_said_biomes.size()) {
                        if (inserted) {
                            if (distances_to_said_biomes[counter] > distances_to_said_biomes[0] + melt_distance) {
                                // this distance, and distances past it are no longer valid
                                distances_to_said_biomes.erase(distances_to_said_biomes.begin() + counter, distances_to_said_biomes.end());
                                picked_biomes.erase(picked_biomes.begin()+ counter, picked_biomes.end());
                                break;
                            }
                        }
                        else if (distance < distances_to_said_biomes[counter]) {
                            // insert into our list at this index
                            distances_to_said_biomes.insert(distances_to_said_biomes.begin() + counter, distance);
                            picked_biomes.insert(picked_biomes.begin() + counter, state.island_biome_points[other_sac][i]);
                            inserted = true;
                        }
                        counter++;
                    }
                }
            }
        }
    }
    
    float weight_sum = 0;
    float empty_weight_sum = 0;
    for (int i = 0; i < picked_biomes.size(); i++) {
        float weight = 1.0f - (distances_to_said_biomes[i] - distances_to_said_biomes[0]) / melt_distance;
        if (picked_biomes[i]) {
            weight_sum += weight;
        }
        else {
            empty_weight_sum += weight;
        }
    }
    // what fraction of this island is actual island and not an edge
    // 0 -> we're at edge of island
    // 1 -> we're a bit far from the edge of the island
    float fraction_island = 1.0f - empty_weight_sum / (weight_sum + empty_weight_sum);
    *lower = 0;
    *upper = 0;
    float lstrength, ustrength, lpers, upers;
    lstrength = ustrength = lpers = upers = 0;
    for (int i = 0; i < picked_biomes.size(); i++) {
        if (picked_biomes[i]) {
            float weight = 1.0f - (distances_to_said_biomes[i] - distances_to_said_biomes[0]) / melt_distance;
            float frac_weight = weight / weight_sum;
            if (weights.count(picked_biomes[i])) {
                weights[picked_biomes[i]] += frac_weight;
            }
            else {
                weights[picked_biomes[i]] = frac_weight;
            }
            
            lstrength += (info->lower_strength) * frac_weight;
            ustrength += (get_biome_strength(picked_biomes[i])) * frac_weight;
            lpers += (info->lower_persistence) * frac_weight;
            upers += (get_biome_persistence(picked_biomes[i])) * frac_weight;
        }
    }
    if (fraction_island > 0.01) {
        float rlower = noise2d(pos.x, pos.z, seed, info->octaves, lpers, lstrength);// * fraction_island;
        rlower -= (info->fatness * fraction_island);
        //rlower += (state.island_heights[sac]);
        float rupper = noise2d(pos.x, pos.z, seed + 1, info->octaves, upers, ustrength);// * fraction_island;
        rupper += (info->fatness * fraction_island);
        //rupper += (state.island_heights[sac]);
        *lower = (int)floorf(rlower);
        *upper = (int)floorf(rupper);
        if (*upper < *lower) {
            *upper = *lower;
        }
    }
    else {
        *lower = CY;
        *upper = 0;
    }
    return false;
}

void fill_chunk_at(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]) {
    //printf("Generating chunk %d %d %d\n", chunk_pos.x, chunk_pos.y, chunk_pos.z);
    get_empty_chunk(to_arr);
    
    if (abs(chunk_pos.y) > 2) {
        return;
    }
    
    int lower, upper;
    std::map<int, float> weights;
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            weights.clear();
            ivec3 pos(x + chunk_pos.x*CX, 0, z + chunk_pos.z*CZ);
            get_heights_at(&lower, &upper, pos, weights);
            int bottom = std::max(lower - chunk_pos.y*CY, 0);
            int top = std::min(upper - chunk_pos.y*CY, CY);
            if (bottom >= top || bottom > CY - 1 || top < 1)
                continue;
            for (int y = bottom; y < top; y++) {
                // hacky, but adding 5 to biome to get block
                srand(seed);
                srand(rand() + chunk_pos.x*CX+x);
                srand(rand() + chunk_pos.y*CY+y);
                srand(rand() + chunk_pos.z*CZ+z);
                uint16_t type = pick_biome_from_weights(weights);
                if (type) {
                    to_arr[x][y][z] = get_random_block_from_biome(type, upper - y-chunk_pos.y*CY - 1);
                }
            }
        }
    }
    //printf("Done\n");
    
    /*
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
    }*/
}

void fill_chunk_at_one_island(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]) {
    
    
}

void clean_world_generator() {
    // nothing needs to be done (yet)
}


void test_world_generator() {
    /*int lower, upper;
    setup_world_generator(100);
    //x=60 to 79, z=68-69 to test boundary
    //test funkiness going on at -569/6/32
    for (int x = -569; x < -566; x++) {
        for (int z = 32; z < 33; z++) {
            ivec3 first(x, 0, z);
            printf("For %d, %d\n", x, z);
            std::map<int, float> weights;
            get_heights_at(&lower, &upper, first, weights);
            printf("%d to %d\n", lower, upper);
            get_heights_at(&lower, &upper, first, weights);
            printf("%d to %d still\n", lower, upper);
        }
    }*/
    srand(200);
    std::map<int, float> weights;
    weights[10] = 0.0f;
    weights[11] = 0.9f;
    weights[12] = 0.1f;
    for (int i = 0; i < 15; i++)
        printf("%d\n", pick_biome_from_weights(weights));
}
