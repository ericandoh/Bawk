//
//  chunk.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "chunk.h"


uint16_t Chunk::get(int x, int y, int z) {
    return blk[x][y][z];
}

void Chunk::set(int x, int y, int z, uint16_t type) {
    blk[x][y][z] = type;
    changed = true;
}