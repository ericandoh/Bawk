//
//  superobjectrender.h
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__superobjectrender__
#define __Bawk__superobjectrender__

#include <stdio.h>
#include <map>
#include "basic_types.h"
#include "chunkrender.h"

typedef std::map<ivec3, RenderableChunk*> chunk_map;

class RenderableSuperObject {
    // note: this may/may NOT contain all data about this superobject
    // instead, pieces of this superobject are actively rendered in/out into this map
    // depending on render distance
    // (we can also optimize it so that active rendered in/out is only done for big objects)
    // (i.e. the world, or MASSIVE SHIPS)
    chunk_map chunks;
public:
    ~RenderableSuperObject();
    void randomize();
    uint16_t get(int x, int y, int z);
    void set(int x, int y, int z, uint16_t type);
    void render(fmat4* transform);
};

#endif /* defined(__Bawk__superobjectrender__) */
