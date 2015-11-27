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
#include "chunkrender.h"

// TODO this whole shit
// this will make it so chunks arent stored in heap and keep memory allocating/deallocating
// fixed size

RenderableChunk* create_new_chunk(block_type raw_chunk[CX][CY][CZ]);

#endif /* defined(__Bawk__chunkresourcemanager__) */
