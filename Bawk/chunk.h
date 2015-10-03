//
//  chunk.h
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__chunk__
#define __Bawk__chunk__

#include <stdio.h>
#include "chunkrender.h"

class Chunk: public RenderableChunk {
public:
    uint16_t get(int x, int y, int z);
    void set(int x, int y, int z, uint16_t type);
};

#endif /* defined(__Bawk__chunk__) */
