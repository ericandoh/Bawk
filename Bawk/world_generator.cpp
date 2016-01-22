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

// holds state of the world generation, for easier generation
struct WorldGeneratorState {
    // biome points per sector (sector pos (QPC) TO position within sector(RWC))
    std::unordered_map<ivec3, std::vector<ivec3>> biome_points;
    // what biome it corresponds to
    std::unordered_map<ivec3, std::vector<int>> island_biome_points;
    // the height of the island at that sector
    std::unordered_map<ivec3, int> island_heights;
};

typedef std::map<int, float> biomedist;

struct chunk_structure_positions {
    std::vector<uint16_t> sids;
    std::vector<ivec3> positions;   // this is centered btw, not at 0,0,0
    std::vector<ivec3> dimensions;
};

struct chunk_heights {
    ivec3 chunk_pos;
    long last_used;
    int lowers[CX][CZ];
    int uppers[CX][CZ];
    biomedist weights[CX][CY];
    chunk_structure_positions structure_positions;  // for structures ONLY centered inside this chunk
                                                    // this chunk MIGHT have periphery structures that are centered in other chunks
                                                    // but bleed into this chunk
};

world_gen_mode_info* info;
WorldGeneratorState state;

const int chunk_height_cache_size = 40;
chunk_heights chunk_height_cache[chunk_height_cache_size];

long last_time = 0;
int seed;

void setup_world_generator(world_gen_mode_info* inf, int s) {
    info = inf;
    seed = s;
    
    for (int i = 0; i < chunk_height_cache_size; i++) {
        chunk_height_cache[i].last_used = -1;
    }
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

int pick_biome_from_weights_deprecated(std::map<int, float> &weights) {
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
        state.island_biome_points[sector_pos][use_index] = pick_biome_from_weights_deprecated(info->biome_frequencies);
        
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
            ustrength += (get_biome(picked_biomes[i])->strength) * frac_weight;
            lpers += (info->lower_persistence) * frac_weight;
            upers += (get_biome(picked_biomes[i])->persistence) * frac_weight;
        }
    }
    if (fraction_island > 0.01) {
        float rlower = -noise2d(pos.x, pos.z, seed, info->octaves, lpers, lstrength);// * fraction_island;
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

chunk_heights* get_chunk_height_cached(ivec3 chunk_pos) {
    chunk_pos.y = 0;
    long lowest_last_used = last_time + 1;
    int j = -1;
    for (int i = 0; i < chunk_height_cache_size; i++) {
        if (chunk_height_cache[i].last_used < 0) {
            j = i;
            break;
        }
        else if (chunk_height_cache[i].chunk_pos == chunk_pos) {
            chunk_height_cache[i].last_used = last_time;
            last_time++;
            return &(chunk_height_cache[i]);
        }
        else if (j < 0 || chunk_height_cache[i].last_used < lowest_last_used) {
            j = i;
            lowest_last_used = chunk_height_cache[i].last_used;
        }
    }
    
    if (j < 0)
        j = 0;
    
    chunk_height_cache[j].last_used = last_time;
    chunk_height_cache[j].chunk_pos = chunk_pos;
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            chunk_height_cache[j].weights[x][z].clear();
        }
    }
    last_time++;
    // generate heightmap and chunk biome weights
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            ivec3 pos(x + chunk_pos.x*CX, 0, z + chunk_pos.z*CZ);
            int lower, upper;
            get_heights_at(&(lower),
                           &(upper),
                           pos, chunk_height_cache[j].weights[x][z]);
            chunk_height_cache[j].lowers[x][z] = lower;
            chunk_height_cache[j].uppers[x][z] = upper;
            // now for each position, decide if we want a structure centered there or not
            if (chunk_height_cache[j].weights[x][z].size() == 1 && lower < upper) {
                // we have one and only one biome is the sufficient and necessary condition to generate a structure
                // we might change this later depending on how we feel like
                biomedist::iterator it = chunk_height_cache[j].weights[x][z].begin();
                int biome = it->first;
                srand(seed+(chunk_pos.x*CX+x)*2);
                srand(rand() + (chunk_pos.z*CZ+z)*2);
                add_struct_in_biome_randomly(biome,ivec3(chunk_pos.x*CX+x,upper,chunk_pos.z*CZ+z),
                                             chunk_height_cache[j].structure_positions.sids,
                                             chunk_height_cache[j].structure_positions.dimensions,
                                             chunk_height_cache[j].structure_positions.positions);
            }
            // TODO check why we have places where upper > lower but we have one biome...thats hellava strange
        }
    }
    return &(chunk_height_cache[j]);
}

void get_chunk_structures_in_range(chunk_structure_positions* dst, ivec3 chunk_pos) {
    
    bounding_box chunk_bb(fvec3(chunk_pos.x*CX,chunk_pos.y*CY,chunk_pos.z*CZ),
                          fvec3(chunk_pos.x*CX+CX,chunk_pos.y*CY+CY,chunk_pos.z*CZ+CZ));
    
    // now iterate through our chunks around our chunk
    // this part restricts our structures from being greater than
    for (int xoff = -1; xoff <= 1; xoff++) {
        for (int zoff = -1; zoff <=1 ; zoff++) {
            chunk_heights* at = get_chunk_height_cached(ivec3(chunk_pos.x + xoff,
                                                              0,
                                                              chunk_pos.z + zoff));
            for (int i = 0 ; i < at->structure_positions.sids.size(); i++) {
                ivec3 pos = at->structure_positions.positions[i];
                ivec3 dimensions = at->structure_positions.dimensions[i];
                bounding_box struct_bb(fvec3(pos.x,pos.y,pos.z),
                                       fvec3(pos.x+dimensions.x,
                                             pos.y+dimensions.y,
                                             pos.z+dimensions.z));
                if (chunk_bb.hits(struct_bb)) {
                    // this structure is within our chunk bounding box
                    dst->sids.push_back(at->structure_positions.sids[i]);
                    dst->positions.push_back(at->structure_positions.positions[i]);
                    dst->dimensions.push_back(at->structure_positions.dimensions[i]);
                }
            }
        }
    }
}

void fill_chunk_at(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]) {
    //printf("Generating chunk %d %d %d\n", chunk_pos.x, chunk_pos.y, chunk_pos.z);
    get_empty_chunk(to_arr);
    
    if (abs(chunk_pos.y) > 2) {
        return;
    }
    
    chunk_heights* height = get_chunk_height_cached(chunk_pos);
    
    srand(seed);
    srand(rand() + chunk_pos.x*CX);
    srand(rand() + chunk_pos.y*CY);
    srand(rand() + chunk_pos.z*CZ);
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            ivec3 pos(x + chunk_pos.x*CX, 0, z + chunk_pos.z*CZ);
            int lower = height->lowers[x][z];
            int upper = height->uppers[x][z];
            int bottom = std::max(lower - chunk_pos.y*CY, 0);
            int top = std::min(upper - chunk_pos.y*CY, CY);
            if (bottom >= top || bottom > CY - 1 || top < 1)
                continue;
            for (int y = bottom; y < top; y++) {
                uint16_t type = pick_biome_from_weights_deprecated(height->weights[x][z]);
                if (type) {
                    to_arr[x][y][z] = get_biome(type)->get_random_block(upper - y-chunk_pos.y*CY - 1);
                }
            }
        }
    }
    
    // now add in structures
    chunk_structure_positions positions;
    get_chunk_structures_in_range(&positions, chunk_pos);
    for (int i = 0; i < positions.sids.size(); i++) {
        add_recipe_block_to_chunk(to_arr, positions.sids[i],
                                  chunk_pos, positions.positions[i]);
        
    }
    //printf("Done\n");
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
        printf("%d\n", pick_biome_from_weights_deprecated(weights));
}
