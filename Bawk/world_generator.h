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

void setup_world_generator(int fillthis);
void clean_world_generator();

// fills the general player world with new chunks/islands
void fill_chunk_at(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]);
// fills a portal map with one island of content, for farming
void fill_chunk_at_one_island(ivec3 chunk_pos, block_type to_arr[CX][CY][CZ]);

#endif /* defined(__Bawk__world_generator__) */
