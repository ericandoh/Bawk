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

// --- SuperObject ---
void PlaceableSuperObject::set_block(float x, float y, float z, block_type type) {
    if (get_block_independence(get_block(x, y, z).type)) {
        makes_vehicle--;
    }
    SuperObject::set_block(x, y, z, type);
    // see if this block is a vehicle block
    if (get_block_independence(type.type)) {
        makes_vehicle++;
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