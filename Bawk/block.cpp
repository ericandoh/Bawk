//
//  block.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "block.h"

int get_transparency(uint16_t block) {
    if (block == 0) {
        return 2;
    }
    // all blocks are opaque for now
    // later return an integer if not transparent
    return 0;
}

int creates_independence(uint16_t block) {
    return 0;
}