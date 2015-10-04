//
//  block_loader.h
//  Bawk
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

void make_world_folder(std::string world_name);
void make_object_in_world_folder(std::string world_name, std::string object_name);
int save_chunk_to_file(std::string world_name,
                std::string object_name,
                ivec3* chunk_pos,
                uint16_t block[CX][CY][CZ]);
int load_chunk_from_file(std::string world_name,
                std::string object_name,
                ivec3* chunk_pos,
                uint16_t block[CX][CY][CZ]);

#endif /* defined(__Bawk__block_loader__) */
