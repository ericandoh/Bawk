//
//  block.h
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__block__
#define __Bawk__block__

#include <stdio.h>
#include <stdint.h>
#include "player.h"

// orientation, from a viewer looking from the positive x-direction
enum BlockOrientation: uint8_t {
    FRONT = 0,
    TOP = 1,
    RIGHT = 2,
    BACK = 3,
    BOTTOM = 4,
    LEFT = 5
};

struct block_type {
    // the physical type of the block
    uint16_t type;
    // the orientation of the block
    BlockOrientation orientation;
    // the health of the block.
    // as the block takes more damage, INCREMENT this value (life starts at 0 and goes up)
    // (its kind of like aging okay?)
    uint16_t life;
    uint64_t block_id;
    Player* owner;
    block_type();
    // used by world to make generic blocks
    block_type(uint16_t t);
    // used by player when placing a block
    block_type(uint16_t t, BlockOrientation orient, Player* me);
    // used by world loader when loading a pre-existing memory
    block_type(uint16_t t, BlockOrientation orient, uint16_t h, uint64_t bid, Player* me);
    // two blocks are equal if they have the same type, orientation, and life
    // (those are the factors that will affect rendering)
    bool equals(block_type other);
};

// gets the transparency of a block. 0 is opaque
int get_transparency(uint16_t block);

// if a block is independent, any template made with it will become an independent superobject
int creates_independence(uint16_t block);

#endif /* defined(__Bawk__block__) */
