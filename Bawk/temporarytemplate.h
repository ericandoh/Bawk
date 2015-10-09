//
//  temporarytemplate.h
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__temporarytemplate__
#define __Bawk__temporarytemplate__

#include <stdio.h>
#include <vector>
#include "basic_types.h"
#include "world.h"

struct block_data {
    ivec3 position;
    uint16_t block;
    block_data(ivec3 pos, uint16_t b) {
        position= pos;
        block = b;
    }
};

// represents a template in the making
class TemporaryTemplate {
    std::vector<block_data> blocks;
    // if this is toggled, when finalizing add blocks not to the world
    // but to its own independent superobject
    int will_be_independent;
public:
    TemporaryTemplate();
    void add_block(ivec3 position, uint16_t block);
    void remove_block(ivec3 position);
    std::vector<block_data> publish(World* world);
    void unpublish(World* world);
};

#endif /* defined(__Bawk__temporarytemplate__) */
