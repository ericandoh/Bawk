//
//  Loads info about the game, like what block types have what definitions
//  - Unlike block_loader, loads/saves in plaintext format
//  - Later can make it compile a plaintext file into a data file to be used by the game
//  - (if we want to make the game files non-edittable, which is unlikely bc EDIT ALL THE THINGS)
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//  Created by Eric Oh on 10/22/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game_info_loader__
#define __Bawk__game_info_loader__

#include <stdio.h>
#include <vector>
#include <map>
#include "basic_types.h"
#include "block.h"
#include "blockaction.h"
#include "game_actions.h"
#include "lightrender.h"
#include "settings.h"

class CursorItem;
class RenderableModel;
class SpriteRender;

namespace Json {
    class Value;
}

int load_game_info();
void free_game_info();

uint16_t get_block_texture(block_type blk, BlockOrientation face);
// deprecate below
bool get_block_is_model(uint16_t block_id);
RenderableLight* get_block_light(uint16_t block_id);
int get_block_resistance(uint16_t block_id);
int get_block_transparency(uint16_t block_id);
int get_block_weight(uint16_t block_id);
int get_block_independence(uint16_t block_id);

RenderableModel* get_game_model(uint16_t model_id);

bool call_block_mouse_callback_from(block_type* block_id, Game* game, Entity* ent, ivec3 pos, Action a);
void get_block_keyboard_callback_from(block_type block_id, std::map<Action, block_callback_func> &funcs);

bool has_block_mouse_action(block_type block_id);
bool has_block_keyboard_action(block_type block_id);

SpriteRender* get_sprite_renderable(uint16_t sid);

uint16_t get_model_id_from_name(Json::Value node);

CursorItem* get_recipe_cursoritem_from(uint16_t vid);

// some world generation methods below

void fill_game_models(std::vector<fvec3> &model_vertices,
                      std::vector<fvec3> &model_normals,
                      std::vector<fvec3> &model_uvs,
                      block_type block,
                      int x, int y, int z);

float get_biome_strength(uint16_t biome);
float get_biome_persistence(uint16_t biome);
uint16_t get_random_block_from_biome(uint16_t biome, int depth);
void add_struct_in_biome_randomly(uint16_t biome, ivec3 pos,
                                  std::vector<uint16_t> &sids,
                                  std::vector<ivec3> &dimensions,
                                  std::vector<ivec3> &positions);
// adds the recipe ID
void add_recipe_block_to_chunk(block_type to_arr[CX][CY][CZ], uint16_t recipe,
                               ivec3 chunk_pos, ivec3 recipe_pos);
#endif /* defined(__Bawk__game_info_loader__) */
