//
//  placeablesuperobject.cpp
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "placeablesuperobject.h"
#include "game_info_loader.h"
#include "world.h"
#include "gametemplate.h"
#include "game.h"

// for game templates
PlaceableSuperObject::PlaceableSuperObject(): SuperObject() {
    vid = 0;
    pid = 0;
    makes_vehicle = 0;
    can_rotate = true;
}

// for recipes
PlaceableSuperObject::PlaceableSuperObject(uint32_t s): SuperObject() {
    vid = s;
    pid = 0;
    makes_vehicle = 0;
    can_rotate = true;
}

// for custom cursorsuperobjects
PlaceableSuperObject::PlaceableSuperObject(uint32_t p, uint32_t s): SuperObject(p, s) {
    makes_vehicle = 0;
    can_rotate = true;
}

fvec3 PlaceableSuperObject::calculate_center_position() {
    // calculate the center position of this object
    
    // first, find direction we're pointing in
    // 0 - x, 1 - y, 2 - z
    int pointing_in = 0;
    fvec3 aligned_upper = fvec3(bounds.upper.x - bounds.lower.x,
                                bounds.upper.y - bounds.lower.y,
                                bounds.upper.z - bounds.lower.z);
    bool align_to_half = false;
    // next, find out if we round to nearest x.5 or to nearest x.0
    if (pointing_in == 0) {
        // use y
        int rounded_val = roundf(aligned_upper.y * 2.0);
        if (rounded_val % 2 == 1) {
            align_to_half = true;
        }
    }
    else if (pointing_in == 1) {
        // use x
        int rounded_val = roundf(aligned_upper.x * 2.0);
        if (rounded_val % 2 == 1) {
            align_to_half = true;
        }
    }
    else {
        // we're aligned in z-direction, wtf!!!
        // use z
        int rounded_val = roundf(aligned_upper.z * 2.0);
        if (rounded_val % 2 == 1) {
            align_to_half = true;
        }
    }
    
    if (align_to_half) {
        fvec3 result(roundf(aligned_upper.x / 2.0 - 0.5) + 0.5,
                     roundf(aligned_upper.y / 2.0 - 0.5) + 0.5,
                     roundf(aligned_upper.z / 2.0 - 0.5) + 0.5);
        return result;
    }
    else {
        fvec3 result(roundf(aligned_upper.x / 2.0),
                     roundf(aligned_upper.y / 2.0),
                     roundf(aligned_upper.z / 2.0));
        return result;
    }
}

// --- SuperObject ---
void PlaceableSuperObject::handle_block_addition(float x, float y, float z, block_type type) {
    SuperObject::handle_block_addition(x, y, z, type);
    if (get_block_independence(type.type)) {
        makes_vehicle++;
    }
}

void PlaceableSuperObject::handle_block_removal(float x, float y, float z, block_type type) {
    SuperObject::handle_block_removal(x, y, z, type);
    if (get_block_independence(get_block(x, y, z).type)) {
        makes_vehicle--;
    }
}

void PlaceableSuperObject::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    // always render all objects in PlaceableSuperObject
    // do nothing
}

// --- PlaceableObject ---
bool PlaceableSuperObject::set_blocks(Game* game) {
    if (game->game_template) {
        // we're still setting our blocks into a game template
        return set_blocks(game->player, game->world, game->game_template);
    }
    else {
        if (makes_vehicle) {
            SuperObject* obj = game->world->create_superobject(game->player);
            obj->pos = fvec3(this->pos.x + this->bounds.lower.x,
                             this->pos.y + this->bounds.lower.y,
                             this->pos.z + this->bounds.lower.z);
            obj->center_pos = calculate_center_position();
            if (set_blocks(game->player, game->world, obj)) {
                return true;
            }
            else {
                game->world->remove_entity(obj);
                delete obj;
                return false;
            }
        }
        else {
            return set_blocks(game->player, game->world, game->world->base_world);
        }
    }
}

bool PlaceableSuperObject::set_blocks(Player* player, World* world, SuperObject* object) {
    // TODO if we rotated this object, update individual block orientations
    if (world->will_collide_with_anything(this)) {
        return false;
    }
    printf("Publishing blocks!\n");
    int counter = 0;
    for (auto &i: chunk_bounds) {
        RenderableChunk* chunk = 0;
        if (chunks.count(i.first)) {
            chunk = chunks[i.first];
        }
        else {
            if (load_chunk(i.first.x, i.first.y, i.first.z)) {
                // chunk could not be loaded
                continue;
            }
            chunk = chunks[i.first];
        }
        for (int x = 0; x < CX; x++) {
            for (int y = 0; y < CY; y++) {
                for (int z = 0; z < CZ; z++) {
                    block_type block = chunk->blk[x][y][z];
                    if (block.type) {
                        counter++;
                        fvec3 world_coord;
                        ivec3 chunk_pos = i.first;
                        transform_into_world_coordinates(&world_coord,
                                                         chunk_pos.x*CX+x,
                                                         chunk_pos.y*CY+y,
                                                         chunk_pos.z*CZ+z);
                        ivec3 block_pos = ivec3(int(world_coord.x),
                                                int(world_coord.y),
                                                int(world_coord.z));
                        block.owner = player->getID();
                        object->set_block(block_pos.x, block_pos.y, block_pos.z, block);
                    }
                }
            }
        }
    }
    printf("Set %d blocks\n", counter);
    return true;
}

// --- PlaceableSuperObject ---
void PlaceableSuperObject::move_template(ivec3 dir) {
    pos.x += dir.x;
    pos.y += dir.y;
    pos.z += dir.z;
}

void PlaceableSuperObject::rotate_template() {
    printf("frog\n");
}

void PlaceableSuperObject::render_blocks(fmat4* transform) {
    render(transform);
}