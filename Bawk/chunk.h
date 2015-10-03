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
    int get_transparency(uint16_t block) override;
};

#endif /* defined(__Bawk__chunk__) */
