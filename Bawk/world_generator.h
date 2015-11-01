//
//  world_generator.h
//  Bawk
//
//  Created by Eric Oh on 10/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__world_generator__
#define __Bawk__world_generator__

#include <stdio.h>
#include "basic_types.h"
#include "chunkrender.h"
#include "world_generator_structures.h"

void setup_world_generator(world_gen_mode_info* inf, int s);
void clean_world_generator();

// fills the general player world with new chunks/islands
void fill_chunk_at(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]);

void test_world_generator();

#endif /* defined(__Bawk__world_generator__) */
