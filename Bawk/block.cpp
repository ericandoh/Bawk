//
//  block.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "block.h"

block_type::block_type() {
    type = 0;
    orientation = DEFAULT_BLOCK_ORIENTATION;
    life = 0;
    block_id = 0;
    owner = 0;
    is_recipe = 0;
    relx = rely = relz = 0;
}

block_type::block_type(uint16_t t) {
    type = t;
    orientation = DEFAULT_BLOCK_ORIENTATION;
    life = 0;
    block_id = 0;
    owner = 0;
    is_recipe = 0;
    relx = rely = relz = 0;
}

block_type::block_type(uint16_t t, BlockOrientation orient, uint32_t pid) {
    type = t;
    orientation = orient;
    life = 0;
    block_id = 0;
    owner = pid;
    is_recipe = 0;
    relx = rely = relz = 0;
}

block_type::block_type(uint16_t t, BlockOrientation orient, uint16_t h, uint64_t bid, uint32_t pid) {
    type = t;
    orientation = orient;
    life = h;
    block_id = bid;
    owner = pid;
    is_recipe = 0;
    relx = rely = relz = 0;
}

bool block_type::equals(block_type other) {
    return type == other.type && orientation == other.orientation && life == other.life && (!is_recipe);
}

int creates_independence(uint16_t block) {
    return 0;
}