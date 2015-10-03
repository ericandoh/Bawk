//
//  Superobject.h
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__Superobject__
#define __Bawk__Superobject__

#include <stdio.h>

#include <map>
#include "basic_types.h"
#include "chunk.h"

typedef std::map<ivec3, Chunk*> chunk_map;

class SuperObject {
    // note: this may/may NOT contain all data about this superobject
    // instead, pieces of this superobject are actively rendered in/out into this map
    // depending on render distance
    // (we can also optimize it so that active rendered in/out is only done for big objects)
    // (i.e. the world, or MASSIVE SHIPS)
    chunk_map chunks;
public:
    // free chunks related with this superobject
    ~SuperObject();
    // fills superobject with some data. Used only for initial test, might replace later
    void fill();
    uint8_t get(int x, int y, int z);
    void set(int x, int y, int z, uint8_t type);
    void render(fmat4* transform);
};

#endif /* defined(__Bawk__Superobject__) */
