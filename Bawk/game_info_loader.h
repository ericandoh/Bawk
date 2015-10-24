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
#include "basic_types.h"
#include "block.h"

class CursorItem;

int load_game_info();
void free_game_info();

uint16_t get_block_texture(block_type blk, BlockOrientation face);
int get_block_resistance(uint16_t block_id);
int get_block_transparency(uint16_t block_id);
int get_block_weight(uint16_t block_id);
int get_block_independence(uint16_t block_id);

CursorItem* get_recipe_cursoritem_from(uint16_t vid);

// some world generation methods below

void fill_game_models(std::vector<fvec3> &model_vertices,
                      std::vector<fvec3> &model_normals,
                      std::vector<fvec3> &model_uvs,
                      block_type block);

#endif /* defined(__Bawk__game_info_loader__) */
