//
//  The world, excluding vehicles. Only includes world blocks
//  - Is NOT the whole world. For the entire world see world.h
//  -
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__base_world__
#define __Bawk__base_world__

#include <stdio.h>
#include <string>
#include <unordered_map>
#include "superobject.h"

class BaseWorld: public SuperObject {
    block_type air[CX][CY][CZ];
    block_type ground[CX][CY][CZ];
    
    uint16_t sector_generator_id;
    std::unordered_map<ivec3, int> sector_loaded_map;
    
public:
    BaseWorld();
    // --- SuperObject ---
    RenderableChunk* get_chunk(int x, int y, int z) override;
    
    bool within_dimensions_chunk(int x, int y, int z) override;
    bool intersects_chunk(ivec3 lower, ivec3 upper, ivec3 chunkpos) override;
    void update_dimensions_from_chunk(ivec3 chunk_pos) override;
    
    void update_render(fvec3* player_pos) override;
    
    // --- Entity ---
    void step(int ms) override;
    bool collides_with(Entity* other) override;
    
    int load_self(IODataObject* obj) override;
    void save_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__base_world__) */
