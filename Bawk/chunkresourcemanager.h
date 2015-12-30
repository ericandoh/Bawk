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

class RenderableSuperObject;

// blocks the thread
RenderableChunk* reserve_chunk_resource(RenderableSuperObject* owner, ivec3 pos);
RenderableChunk* reserve_empty_chunk_resource(RenderableSuperObject* owner, ivec3 pos);
void save_chunk_resource(RenderableChunk* chunk);
void free_chunk_resource(RenderableChunk* chunk);
void save_all_chunk_resources();

#endif /* defined(__Bawk__chunkresourcemanager__) */
