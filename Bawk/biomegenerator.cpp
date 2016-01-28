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
#include "cursorsuperobject.h"
#include "game_info_loader.h"

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
    float rv = choose_random_one();
    float total = 0;
    for (auto &i: structures) {
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
            // if the biome at this index isn't equal to ours, skip
            if (sector_info->biome_map[x][z].biomes[0] != biome_id) {
                continue;
            }
            if (sector_info->biome_map[x][z].biome_weights[0] < 0.7f) {
                // biome isn't strong enough
                continue;
            }
            uint16_t struct_id = get_random_struct(&depth);
            if (struct_id > 0) {
                if (!structure_models.count(struct_id)) {
                    CursorSuperObject* item = get_recipe_cursoritem_from(struct_id);
                    structure_models[struct_id] = item;
                }
                CursorSuperObject* model = structure_models[struct_id];
                int upper = sector_info->biome_map[x][z].upper;
                model->set_pos(fvec3(x + sector_info->sector_bounds.lower.x,
                                     depth + upper + 1,
                                     z + sector_info->sector_bounds.lower.z));
                // the 0 is for the player ID of the blocks, under owner
                model->copy_into(0, sector_info->target);
                // enable this if we're also putting actual models into the world
                /*if (model->set_blocks(game->player, game->world, target)) {
                    locked = false;
                }*/
                
                // add struct at depth
                //sector_info->set_block(ivec3 pos, block_type blk);
            }
        }
    }
    
    // find # of structures to add depending on the bounding box
    // pick points that won't overlap
    // add structures
}

void BiomeGenerator::reset() {
    for (auto &i: structure_models) {
        delete i.second;
    }
    structure_models.clear();
}
    