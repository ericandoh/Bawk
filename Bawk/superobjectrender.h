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
#include <unordered_map>
#include "basic_types.h"
#include "chunkrender.h"

namespace std
{
    template<>
    struct hash<ivec3> {
        size_t operator()(const ivec3 &pt) const {
            // turn first 9 bits to a 27 bit string, then add together
            // the result should be XYZXYZXYZXYZ for XXXX, YYYY, ZZZZ (but with more bits)
            uint32_t xb = spread_bits(pt.x);
            uint32_t yb = spread_bits(pt.y) << 1;
            uint32_t zb = spread_bits(pt.z) << 2;
            return xb + yb + zb;
        }
    };
}

typedef std::unordered_map<ivec3, RenderableChunk*> chunk_map;

class RenderableSuperObject {
    // note: this may/may NOT contain all data about this superobject
    // instead, pieces of this superobject are actively rendered in/out into this map
    // depending on render distance
    // (we can also optimize it so that active rendered in/out is only done for big objects)
    // (i.e. the world, or MASSIVE SHIPS)
    chunk_map chunks;
    // internal function that will make calls to load and save a chunk
    int load_chunk(int x, int y, int z);
    void delete_chunk(int x, int y, int z);
public:
    void remove_self();
    uint16_t get_block(int x, int y, int z);
    void set_block(int x, int y, int z, uint16_t type);
    void render(fmat4* transform);
    // given player coordinates loads in new chunks/deprecates old ones
    void update_chunks(fvec3* old_pos, fvec3* new_pos);
    // called when a request to load a chunk from disk is made
    virtual int get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) = 0;
    // called when a chunk goes out of scope and no longer needs to be rendered
    virtual int save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) = 0;
    // checks if a chunk at x, y, z is within the superobject dimensions
    virtual bool within_dimensions(int x, int y, int z) = 0;
    // updates the superobject dimensions
    virtual void update_dimensions(ivec3* pos) = 0;
};

#endif /* defined(__Bawk__superobjectrender__) */
