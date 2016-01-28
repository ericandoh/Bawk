//
//  sectorgenerator.cpp
//  Bawk
//
//  Created by Eric Oh on 1/14/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "sectorgenerator.h"
#include <vector>
#include <unordered_map>
#include <glm/gtc/noise.hpp>
#include "settings.h"
#include "superobject.h"
#include "game_info_loader.h"

#include "biomegenerator.h"

struct ChunkData {
    block_type blks[CX][CY][CZ];
};

SectorGenerationInfo::SectorGenerationInfo() {
    
}

SectorGenerationInfo::~SectorGenerationInfo() {
    for (auto &i: sector_chunks) {
        delete i.second;
    }
    sector_chunks.clear();
    biome_map.clear();
}

// --- RenderableSuperObject
void transform_into_chunk_bounds_bad_programming_practice(ivec3* cac,
                                                        ivec3* crc,
                                                        float x, float y, float z) {
    int xr = int(floorf(x));
    int yr = int(floorf(y));
    int zr = int(floorf(z));
    // chunk relative coordinates
    crc->x = (xr % CX + CX) % CX;
    crc->y = (yr % CY + CY) % CY;
    crc->z = (zr % CZ + CZ) % CZ;
    
    // chunk aligned coordinates
    cac->x = (xr - crc->x) / CX;
    cac->y = (yr - crc->y) / CY;
    cac->z = (zr - crc->z) / CZ;
}

void SectorGenerationInfo::set_block(ivec3 pos, block_type blk) {
    ivec3 cac, crc;
    transform_into_chunk_bounds_bad_programming_practice(&cac, &crc,
                                                         pos.x,// + bounds.lower.x,
                                                         pos.y,
                                                         pos.z);// + bounds.lower.z);
    if (sector_chunks.count(cac)) {
        // this chunk already made
    }
    else {
        sector_chunks[cac] = new ChunkData();
        // clear blks
        get_empty_chunk(sector_chunks[cac]->blks);
    }
    ChunkData* ref = sector_chunks[cac];
    
    ref->blks[crc.x][crc.y][crc.z] = blk;
}

SectorGenerator::SectorGenerator() {
    // these will later be overwritten by a JSON reader
    name = "";
    sector_xwidth = sector_zwidth = 64; //= 1024;
    fill_perc_avg = 0.5f;
    fill_perc_var = 0.1f;
    biome_count_avg = 5;
    biome_count_var = 0;
    island_count_avg = 3;
    island_count_var = 0;
    melt_distance = 4.0f;
    seed = 0;
}

bool SectorGenerator::can_create_sector(ivec3 sector_pos) {
    // later replace this with info from JSON
    if (sector_pos == ivec3(0,0,0)) {
        return true;
    }
    return false;
    /*float dst = get_ivec3_distance(sub_ivec3(sector_pos, ivec3(0,0,0)));
    // return true if player's level > x as distance from sector...
    return dst < 3;*/
}

ivec3 SectorGenerator::transform_into_sector_pos(ivec3 chunk_pos) {
    int rwc_x = chunk_pos.x * CX;
    int rwc_z = chunk_pos.z * CZ;
    
    int offset_x = sector_xwidth / 2;
    int offset_z = sector_zwidth / 2;
    
    ivec3 sector_pos;
    sector_pos.x = floorf((rwc_x + offset_x) * 1.0f / sector_xwidth);
    sector_pos.y = 0;
    sector_pos.z = floorf((rwc_z + offset_z) * 1.0f / sector_zwidth);
    return sector_pos;
}

int_bounding_box SectorGenerator::transform_into_bounds(ivec3 sector_pos) {
    int offset_x = sector_xwidth / 2;
    int offset_z = sector_zwidth / 2;
    
    int_bounding_box result;
    
    result.lower.x = (sector_pos.x) * sector_xwidth - offset_x;
    result.lower.y = 0;
    result.lower.z = (sector_pos.z) * sector_zwidth - offset_z;
    
    result.upper.x = (sector_pos.x + 1) * sector_xwidth - offset_x;
    result.upper.y = 0;
    result.upper.z = (sector_pos.z + 1) * sector_zwidth - offset_z;
    
    return result;
}

// called when a chunk doesn't exist in memory, see if we "can" create the sector needed for it
bool SectorGenerator::try_create_sector(ivec3 chunk_pos, SuperObject* obj) {
    ivec3 sector_pos = transform_into_sector_pos(chunk_pos);
    if (!can_create_sector(sector_pos)) {
        return false;
    }
    // TODO put this into main pipeline instead and make it show a loading screen for the duration
    generate_sector(sector_pos, obj);
    return true;
}

// --- GENERATION ---

struct BiomePoint {
    // in RWC, aligned to sector (so 0,0 to (width, height) of sector)
    ivec3 position;
    // biome id
    int bid;
    // unique id per biome
    BiomeGenerationInfo info;
    BiomePoint() {
        position = ivec3(0,0,0);
        bid = 0;
        info = BiomeGenerationInfo();
    }
    BiomePoint(ivec3 p) {
        position = p;
        bid = 0;
        info = BiomeGenerationInfo();
    }
};

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
    // if weights was a length 0 array, this will be executed
    return 0;
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

void SectorGenerator::generate_sector(ivec3 sector_pos, SuperObject* obj) {
    
    // set seed here
    srand(seed);
    srand(rand() * sector_pos.x);
    srand(rand() * sector_pos.z);
    
    int biome_count = choose_random(biome_count_avg, biome_count_var);
    int island_count = choose_random(island_count_avg, island_count_var);
    float fill_perc = choose_random(fill_perc_avg, fill_perc_var);
    
    // estimate the total number of biome points to generate
    int total_biome_count = (int)(biome_count / fill_perc);
    
    // make the biome points
    std::vector<BiomePoint> biome_points;
    std::vector<BiomePoint> padding_points;
    
    biome_points.reserve(total_biome_count);
    
    // if we generate islands, toggle this
    bool generate_islands = true;
    
    int margin_x = 0;
    int margin_z = 0;
    if (generate_islands) {
        int sideborder = (int)sqrtf((float)total_biome_count) * 2;
        int sideborder_width = sideborder * sector_xwidth / sector_zwidth;
        int sideborder_height = sideborder;
        margin_x = sector_xwidth / sideborder_width;
        margin_z = sector_zwidth / sideborder_height;
        
        padding_points.reserve(2*sideborder_width + 2*sideborder_height);
        
        for (int i = 0; i < sideborder_width; i++) {
            int xstart = i * sector_xwidth / sideborder_width;
            int xend = (i + 1) * sector_xwidth / sideborder_width;
            int zstart = sector_zwidth;
            int zend = sector_zwidth + margin_z;
            
            padding_points.push_back(BiomePoint(ivec3(rand() % (xend - xstart) + xstart,
                                                    0,
                                                    rand() % (zend - zstart) + zstart)));
            
            zstart = -margin_z;
            zend = 0;
            
            padding_points.push_back(BiomePoint(ivec3(rand() % (xend - xstart) + xstart,
                                                    0,
                                                    rand() % (zend - zstart) + zstart)));
        }
        for (int i = 0; i < sideborder_height; i++) {
            int xstart = sector_xwidth;
            int xend = sector_xwidth + margin_x;
            int zstart = i * sector_zwidth / sideborder_height;
            int zend = (i + 1) * sector_zwidth / sideborder_height;
            
            padding_points.push_back(BiomePoint(ivec3(rand() % (xend - xstart) + xstart,
                                                    0,
                                                    rand() % (zend - zstart) + zstart)));
            
            xstart = -margin_x;
            xend = 0;
            
            padding_points.push_back(BiomePoint(ivec3(rand() % (xend - xstart) + xstart,
                                                    0,
                                                    rand() % (zend - zstart) + zstart)));
        }
    }
    
    int xwidth = sector_xwidth - 2 * margin_x;
    int zwidth = sector_zwidth - 2 * margin_z;
    
    // calculate some reasonable measure for minimum biome point separation
    float approx_square_side = 1.0f / sqrtf((float)total_biome_count) * (sector_xwidth + sector_zwidth - 2*margin_x - 2*margin_z) / 2.0f;
    float minimum_separation = approx_square_side / 2.0f;
    
    int biome_point_counter = 0;
    // try 7 times to find a point more than info.min_biome_point_separation away
    int max_attempts = 7;
    
    bool sector_not_filled = true;
    while (sector_not_filled && biome_point_counter < total_biome_count) {
        if (biome_points.empty()) {
            biome_points.push_back(BiomePoint(ivec3(rand() % xwidth + margin_x,
                                                    0,
                                                    rand() % zwidth + margin_z)));
            biome_point_counter++;
        }
        else {
            bool success = true;
            for (int attempts = 0; attempts < max_attempts; attempts++) {
                ivec3 point = ivec3(rand() % xwidth + margin_x,
                                    0,
                                    rand() % zwidth + margin_z);
                success = true;
                for (auto &i: biome_points) {
                    float dst = get_ivec3_distance(sub_ivec3(i.position, point));
                    if (dst < minimum_separation) {
                        success = false;
                        break;
                    }
                }
                if (success) {
                    biome_points.push_back(BiomePoint(point));
                    biome_point_counter++;
                    break;
                }
            }
            if (!success) {
                // all my attempts at adding biome points failed miserably
                // le miserable sad :(
                sector_not_filled = false;
            }
        }
    }
    total_biome_count = biome_point_counter;
    
    std::vector<int> indices;
    indices.resize(biome_points.size());
    for (int i = 0; i < biome_points.size(); i++) {
        indices[i] = i;
    }
    std::vector<int> island_points;
    std::vector<ivec3> island_centers;
    island_points.resize(island_count);
    island_centers.resize(island_count);
    // now pick a few points as the "seed" for our islands
    for (int i = 0; i < island_count; i++) {
        int choice_in_indices = rand() % (indices.size());
        int real_choice = indices[choice_in_indices];
        island_points[i] = real_choice;
        island_centers[i] = biome_points[real_choice].position;
        indices[choice_in_indices] = indices.back();
        indices.pop_back();
    }
    
    // for each island, "absorb" a point and cluster, until the number of total points
    // in all islands is equal to biome_points
    int added_points = island_count;
    int points_per_island = 1;
    int make_biomes = std::min(biome_count, total_biome_count);
    island_points.reserve(make_biomes);
    while (added_points < make_biomes) {
        for (int i = 0; i < island_count; i++) {
            if (added_points >= make_biomes) {
                break;
            }
            
            ivec3 island_center = island_centers[i];
            
            float shortest_dst = sector_xwidth + sector_zwidth;
            int choice = 0;
            for (int j = 0; j < indices.size(); j++) {
                ivec3 point_center = biome_points[indices[j]].position;
                float dst = get_ivec3_distance(sub_ivec3(island_center, point_center));
                if (dst < shortest_dst) {
                    choice = j;
                    shortest_dst = dst;
                }
            }
            
            island_points.push_back(indices[choice]);
            ivec3 chosen_center = biome_points[indices[choice]].position;
            // recalculate island center for this island
            ivec3 new_island_center = ivec3(island_center.x * points_per_island + chosen_center.x,
                                            island_center.y * points_per_island + chosen_center.y,
                                            island_center.z * points_per_island + chosen_center.z);
            new_island_center.x = new_island_center.x / (points_per_island + 1);
            new_island_center.y = new_island_center.y / (points_per_island + 1);
            new_island_center.z = new_island_center.z / (points_per_island + 1);
            
            island_centers[i] = new_island_center;
            
            indices[choice] = indices.back();
            indices.pop_back();
            
            added_points += 1;
        }
        points_per_island += 1;
    }
    
    // used to keep track of all existing biomes generated
    // later iterate over this to do additional per-biome generation
    std::vector<BiomePoint*> island_biome_points;
    island_biome_points.resize(island_points.size());
    
    // give each point in each island a biome
    for (int i = 0; i < island_points.size(); i++) {
        biome_points[island_points[i]].bid = pick_biome_from_weights(biome_frequencies);
        biome_points[island_points[i]].info.unique_id = i;
        island_biome_points[i] = &(biome_points[island_points[i]]);
    }
    
    // now go through every point on the map and fill in the chunks with the appropriate values
    // also, record for each biome the bounding box of it, and also fill in a biome map
    SectorGenerationInfo* sector_info = new SectorGenerationInfo();
    sector_info->biome_map.resize(sector_xwidth);
    for (int i = 0; i < sector_xwidth; i++) {
        sector_info->biome_map[i].resize(sector_zwidth);
    }
    
    std::vector<BiomePoint> all_points;
    all_points.reserve(biome_points.size() + padding_points.size());
    all_points.insert(all_points.end(), biome_points.begin(), biome_points.end());
    all_points.insert(all_points.end(), padding_points.begin(), padding_points.end());
    
    // TODO this could be more efficient - consider drawing lines through the box
    // at each biome point (in order of x) and then only considering comparing against
    // the biome points on the edges of each slip
    for (int i = 0; i < sector_xwidth; i++) {
        for (int j = 0; j < sector_zwidth; j++) {
            ivec3 point(i, 0, j);
            float shortest_dst = sector_xwidth + sector_zwidth;
            int choice = 0;
            float second_shortest_dst = shortest_dst;
            int second_choice = choice;
            for (int k = 0; k < all_points.size(); k++) {
                float dst = get_ivec3_distance(sub_ivec3(point, all_points[k].position));
                if (dst < shortest_dst) {
                    second_shortest_dst = shortest_dst;
                    second_choice = choice;
                    shortest_dst = dst;
                    choice = k;
                }
                else if (dst < second_shortest_dst) {
                    second_shortest_dst = dst;
                    second_choice = k;
                }
            }
            int bid_choice = all_points[choice].bid;
            int bid_second_choice = all_points[second_choice].bid;
            // now put in the choice into the struct
            sector_info->biome_map[i][j].biomes[0] = bid_choice;
            int unique_id = all_points[choice].info.unique_id;
            sector_info->biome_map[i][j].biome_unique_id = unique_id;
            if (second_shortest_dst < shortest_dst + melt_distance && bid_choice != bid_second_choice) {
                sector_info->biome_map[i][j].biomes[1] = bid_second_choice;
                // weight goes from 0.0 (equal distance) to 1.0 (edge)
                float weight = (second_shortest_dst - shortest_dst) / melt_distance;
                // 0.5 - 1
                sector_info->biome_map[i][j].biome_weights[0] = 0.5f + weight / 2.0f;
                // 0.5 - 0
                sector_info->biome_map[i][j].biome_weights[1] = 0.5f - weight / 2.0f;
            }
            else {
                sector_info->biome_map[i][j].biomes[1] = bid_choice;
                sector_info->biome_map[i][j].biome_weights[0] = 1.0f;
                sector_info->biome_map[i][j].biome_weights[1] = 0.0f;
            }
            // update the biome point itself about info about this point
            if (unique_id >= 0) {
                island_biome_points[unique_id]->info.range.expand(ivec3(i,0,j));
            }
        }
    }
    
    int_bounding_box bounds = transform_into_bounds(sector_pos);
    float strength, persistence;
    float current_fatness;
    for (int x = 0; x < sector_xwidth; x++) {
        for (int z = 0; z < sector_zwidth; z++) {
            biome_position_info info = sector_info->biome_map[x][z];
            // calculate heightmap
            
            float fat;
            fat = info.biomes[0] == 0 ? 0 : fatness / 2.0f;
            current_fatness = fat * info.biome_weights[0];
            fat = info.biomes[1] == 0 ? 0 : fatness / 2.0f;
            current_fatness += fat * info.biome_weights[1];
            
            if (current_fatness > 0) {
                strength = get_biome(info.biomes[0])->strength * info.biome_weights[0];
                strength += get_biome(info.biomes[1])->strength * info.biome_weights[1];
                
                persistence = get_biome(info.biomes[0])->persistence * info.biome_weights[0];
                persistence += get_biome(info.biomes[1])->persistence * info.biome_weights[1];
                
                float rlower = -noise2d(x + bounds.lower.x, z + bounds.lower.z,
                                        seed, octaves, lower_persistence, lower_strength);// * fraction_island;
                rlower -= current_fatness;
                float rupper = noise2d(x + bounds.lower.x, z + bounds.lower.z,
                                       seed + 1, octaves, persistence, strength);// * fraction_island;
                rupper += current_fatness;
                
                sector_info->biome_map[x][z].lower = (int)roundf(rlower);
                sector_info->biome_map[x][z].upper = (int)roundf(rupper) + 1;
                
            }
            else {
                sector_info->biome_map[x][z].lower = 0;
                sector_info->biome_map[x][z].upper = 0;
            }
            
            // also tell each instance of biome about the bounds we have here
        }
    }
    
    //std::unordered_map<ivec3, ChunkData*> sector_chunks;
    
    for (int x = 0; x < sector_xwidth; x++) {
        for (int z = 0; z < sector_zwidth; z++) {
            biome_position_info info = sector_info->biome_map[x][z];
            for (int y = info.lower; y < info.upper; y++) {
                ivec3 cac, crc;
                obj->transform_into_chunk_bounds(&cac, &crc,
                                                 x + bounds.lower.x,
                                                 y,
                                                 z + bounds.lower.z);
                if (sector_info->sector_chunks.count(cac)) {
                    // this chunk already made
                }
                else {
                    sector_info->sector_chunks[cac] = new ChunkData();
                    // clear blks
                    get_empty_chunk(sector_info->sector_chunks[cac]->blks);
                }
                ChunkData* ref = sector_info->sector_chunks[cac];
                int biome_chosen;
                float choice = choose_random_one();
                if (choice < info.biome_weights[0]) {
                    biome_chosen = info.biomes[0];
                }
                else {
                    biome_chosen = info.biomes[1];
                }
                ref->blks[crc.x][crc.y][crc.z] = get_biome(biome_chosen)->get_random_block(info.upper - 1 - y);
            }
        }
    }
    // using the bounding box of each biome, iterate through each biome and then fill in details in that biome
    // using the biomemap from above to see exactly where we have said biomes
    for (int i = 0; i < island_biome_points.size(); i++) {
        int biome_chosen = island_biome_points[i]->info.unique_id;
        get_biome(biome_chosen)->add_structures(sector_info, &island_biome_points[i]->info);
        //island_biome_points[i]->info.range.print_self();
    }
    
    printf("Finished generating sector ");
    printf_ivec3(sector_pos);
    printf("\n");
    
    ivec3 zero(0,0,0);
    validate_read_write_path(obj->get_chunk_save_path(&zero));
    
    // finish and save all chunks to disk
    for (auto &i: sector_info->sector_chunks) {
        ivec3 pos = i.first;
        IODataObject writer(obj->get_chunk_save_path(&pos));
        if (writer.save(false)) {
            // failed to save chunk!!
            continue;
        }
        writer.save_pointer(&(i.second->blks[0][0][0]), sizeof(i.second->blks[0][0][0])*CX*CY*CZ);
        writer.close();
    }
    
    delete sector_info;
    
    /*
    char test_arr[sector_xwidth + margin_x * 2][sector_zwidth + margin_z * 2];
    
    for (int i = -margin_x; i < sector_xwidth + margin_x; i++) {
        for (int j = -margin_z; j < sector_zwidth + margin_z; j++) {
            if (i >= 0 && i < sector_xwidth && j >= 0 && j < sector_zwidth) {
                test_arr[i + margin_x][j + margin_z] = '-';
            }
            else {
                test_arr[i + margin_x][j + margin_z] = '=';
            }
        }
    }
    for (auto &i: padding_points) {
        ivec3 pos = i.position;
        test_arr[pos.x + margin_x][pos.z + margin_z] = 'P';
    }
    for (int i = 0; i < sector_xwidth; i++) {
        for (int j = 0; j < sector_zwidth; j++) {
            if (biome_map[i][j].biomes[0] == 0) {
                continue;
            }
            else if (biome_map[i][j].biome_weights[0] != 1.0f) {
                test_arr[i + margin_x][j + margin_z] = '?';
            }
            else {
                char num = (char)(48 + biome_map[i][j].biomes[0] - 1);
                test_arr[i + margin_x][j + margin_z] = num;
            }
        }
    }

    for (auto &i: biome_points) {
        ivec3 pos = i.position;
        test_arr[pos.x + margin_x][pos.z + margin_z] = 'B';
    }
    for (auto &i: island_points) {
        ivec3 pos = biome_points[i].position;
        test_arr[pos.x + margin_x][pos.z + margin_z] = 'I';
    }
    
    for (int i = -margin_x; i < sector_xwidth + margin_x; i++) {
        for (int j = -margin_z; j < sector_zwidth + margin_z; j++) {
            printf("%c", test_arr[i + margin_x][j + margin_z]);
        }
        printf("\n");
    }
    
    
    printf("done\n");
     */
}


void test_sector_generator() {
    SectorGenerator generator = SectorGenerator();
    generator.generate_sector(ivec3(0,0,0), 0);
}

/*


 */


