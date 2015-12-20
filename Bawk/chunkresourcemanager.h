//
//  chunkresourcemanager.h
//  Bawk
//
//  Created by Eric Oh on 11/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__chunkresourcemanager__
#define __Bawk__chunkresourcemanager__

#include <stdio.h>
#include <string>
#include "chunkrender.h"

// TODO this whole shit
// this will make it so chunks arent stored in heap and keep memory allocating/deallocating
// fixed size

struct ChunkOwnerInformation {
    std::vector<int> placecards;
};

// register a new chunk owner (whether it be the baseworld, some superobject, etc)
int register_chunk_owner(std::string save_path);

RenderableChunk* create_new_chunk(block_type raw_chunk[CX][CY][CZ]);

RenderableChunk* get_chunk_from_placecard(int owner, int placecard);

// some method to get all chunks loaded in for an owner, and call render on them, or some shizzle
ChunkOwnerInformation* get_all_chunks(int owner);

#endif /* defined(__Bawk__chunkresourcemanager__) */
