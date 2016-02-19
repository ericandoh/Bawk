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

#include "blockinfo.h"
#include "biomegenerator.h"
#include "sectorgenerator.h"


class CursorSuperObject;
class RenderableModel;
class SpriteRender;

namespace Json {
    class Value;
}

// --- init/clean ---
int load_game_info();
void free_game_info();

// --- block info ---
BlockInfo* get_block_info(uint16_t block_id);

// deprecate below, or move to a BlockInfo class...
bool call_block_mouse_callback_from(block_type* block_id, Player* player, Entity* ent, ivec3 pos, Action a);
void get_block_keyboard_callback_from(block_type block_id, std::map<Action, block_callback_func> &funcs);

bool has_block_mouse_action(block_type block_id);
bool has_block_keyboard_action(block_type block_id);

// adds block mesh to chunk VBO
void fill_game_models(std::vector<fvec3> &model_vertices,
                      std::vector<fvec3> &model_normals,
                      std::vector<fvec3> &model_uvs,
                      block_type block,
                      int x, int y, int z);

// --- model info ---
RenderableModel* get_game_model(uint16_t model_id);
uint16_t get_model_id_from_name(Json::Value node);

// --- sprite info ---
SpriteRender* get_sprite_renderable(uint16_t sid);

// --- recipe info ---
CursorSuperObject* get_recipe_cursoritem_from(uint16_t vid);

// --- world generation ---

SectorGenerator* get_sector_generator(uint16_t sid);
BiomeGenerator* get_biome(uint16_t biome);

#endif /* defined(__Bawk__game_info_loader__) */
