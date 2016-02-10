//
//  The basic cube unit that makes up this game!
//  - Has a type, orientation (not always needed)
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

#ifndef __Bawk__block__
#define __Bawk__block__

#include <stdio.h>
#include <stdint.h>
#include "block_orientation.h"

struct block_type {
    // the physical type of the block
    // properties such as block resistance, block weight, etc can be calculated from this
    uint16_t type;
    // the orientation of the block
    BlockOrientation orientation;
    // the health of the block.
    // as the block takes more damage, INCREMENT this value (life starts at 0 and goes up)
    // (its kind of like aging okay?)
    uint16_t life;
    // unique id for the block, might differ for different blocks
    uint32_t block_id;
    // id for owner. 0 indicates the WORLD
    uint32_t owner;
    block_type();
    // used by world to make generic blocks
    block_type(uint16_t t);
    // used by player when placing a block
    block_type(uint16_t t, BlockOrientation orient, uint32_t pid);
    // used by world loader when loading a pre-existing memory
    block_type(uint16_t t, BlockOrientation orient, uint16_t h, uint32_t bid, uint32_t pid);
    // two blocks are equal if they have the same type, orientation, and life
    // (those are the factors that will affect rendering)
    bool equals(block_type other);
    bool equals_exact(block_type* other);
};

#endif /* defined(__Bawk__block__) */
