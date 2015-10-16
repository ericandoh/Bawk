//
//  Loads and saves chunk data (blocks)
//  -
//  -
//  -
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
//
//  Created by Eric Oh on 10/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__block_loader__
#define __Bawk__block_loader__

#include <stdio.h>
#include <string>
#include "basic_types.h"
#include "chunkrender.h"    // for CX, CY, CZ
#include "block.h"

void make_world_folder(std::string world_name);
void make_object_in_world_folder(std::string world_name, std::string object_name);
int save_chunk_to_file(std::string world_name,
                std::string object_name,
                ivec3* chunk_pos,
                block_type block[CX][CY][CZ]);
int load_chunk_from_file(std::string world_name,
                std::string object_name,
                ivec3* chunk_pos,
                block_type block[CX][CY][CZ]);

#endif /* defined(__Bawk__block_loader__) */
