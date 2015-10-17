//
//  blockrender.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "blockrender.h"

const unsigned int TILES_PER_TEXTURE = 16;

void set_coord_and_texture(GLbyte coord[][3],
                           GLbyte texture[][3],
                           int index,
                           int x, int y, int z,
                           block_type block, BlockOrientation face) {
    coord[index][0] = x;
    coord[index][1] = y;
    coord[index][2] = z;
    
    // TODO if block has different textures for different faces
    // set them here
    // BlockOrientation transformed_block = transform_orientation(block.orientation, face);
    // uint16_t tile_index = get_tile_index(block.type, transformed_oreintation);
    
    // first 8 bits. Represents the x-axis in our texture atlas
    texture[index][0] = block.type % TILES_PER_TEXTURE;
    // last 8 bits. Represents the y-axis in our texture atlas
    texture[index][1] = block.type / TILES_PER_TEXTURE;
    // some extra flags we can set to let shader know how to render this
    
    GLbyte flags = 0;
    if (face == BlockOrientation::TOP || face == BlockOrientation::BOTTOM) {
        flags += 0x1;
    }
    if (face == block.orientation) {
        flags += 0x2;
    }
    texture[index][2] = flags;
}