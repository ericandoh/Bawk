//
//  block_orientation.cpp
//  Bawk
//
//  Created by Eric Oh on 11/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "block_orientation.h"
#include <glm/gtc/matrix_transform.hpp>


BlockOrientation get_nearest_compass_direction(fvec3 dir) {
    // only look at x, z, disregard y's (for ease of clicking)
    float xabs = fabsf(dir.x);
    float zabs = fabsl(dir.z);
    if (zabs > xabs) {
        // pointing in z direction
        if (dir.z < 0) {
            return BlockOrientation::RIGHT;
        }
        else {
            return BlockOrientation::LEFT;
        }
    } else {
        // pointing in x direction
        if (dir.x < 0) {
            return BlockOrientation::BACK;
        }
        else {
            return BlockOrientation::FRONT;
        }
    }
}


BlockOrientation relative_orientations[BLOCK_ORIENTATION_COUNT][BLOCK_ORIENTATION_COUNT] =
{
    {
        // front (+x)
        BlockOrientation::FRONT,
        BlockOrientation::TOP,
        BlockOrientation::LEFT,
        BlockOrientation::BACK,
        BlockOrientation::BOTTOM,
        BlockOrientation::RIGHT
    },
    {
        // top (+y)
        BlockOrientation::BOTTOM,
        BlockOrientation::FRONT,
        BlockOrientation::LEFT,
        BlockOrientation::TOP,
        BlockOrientation::BACK,
        BlockOrientation::RIGHT
    },
    {
        // left (+z)
        BlockOrientation::RIGHT,
        BlockOrientation::TOP,
        BlockOrientation::BACK,
        BlockOrientation::LEFT,
        BlockOrientation::BOTTOM,
        BlockOrientation::FRONT
    },
    {
        // back (-x)
        BlockOrientation::BACK,
        BlockOrientation::TOP,
        BlockOrientation::RIGHT,
        BlockOrientation::FRONT,
        BlockOrientation::BOTTOM,
        BlockOrientation::LEFT
    },
    {
        // bottom (-y)
        BlockOrientation::TOP,
        BlockOrientation::BACK,
        BlockOrientation::LEFT,
        BlockOrientation::BOTTOM,
        BlockOrientation::FRONT,
        BlockOrientation::RIGHT
    },
    {
        // right (-z)
        BlockOrientation::LEFT,
        BlockOrientation::TOP,
        BlockOrientation::BACK,
        BlockOrientation::RIGHT,
        BlockOrientation::BOTTOM,
        BlockOrientation::FRONT
    },
};

// used for setting individual faces of blocks relative to block local orientation
// base: orientation of the block (where FRONT in block aligned space points in real space)
// dir:  orientation in real space
// RETURN: orientation in block aligned space
BlockOrientation get_translated_orientation(BlockOrientation base, BlockOrientation dir) {
    return relative_orientations[base][dir];
}

/*

// unused code but might be useful later on
 
fvec3 get_vector_from_orientation(BlockOrientation orientation) {
    if (orientation == BlockOrientation::FRONT) {
        return fvec3(1,0,0);
    }
    else if (orientation == BlockOrientation::BACK) {
        return fvec3(-1,0,0);
    }
    else if (orientation == BlockOrientation::LEFT) {
        return fvec3(0,0,1);
    }
    else if (orientation == BlockOrientation::RIGHT) {
        return fvec3(0,0,-1);
    }
    else if (orientation == BlockOrientation::TOP) {
        return fvec3(0,1,0);
    }
    else if (orientation == BlockOrientation::BOTTOM) {
        return fvec3(0,-1,0);
    }
    return fvec3(0,0,0);
}

BlockOrientation get_orientation_from_vector(fvec3 vec) {
    if (vec == fvec3(1,0,0)) {
        return BlockOrientation::FRONT;
    }
    else if (vec == fvec3(-1,0,0)) {
        return BlockOrientation::BACK;
    }
    else if (vec == fvec3(0,0,1)) {
        return BlockOrientation::LEFT;
    }
    else if (vec == fvec3(0,0,-1)) {
        return BlockOrientation::RIGHT;
    }
    else if (vec == fvec3(0,1,0)) {
        return BlockOrientation::TOP;
    }
    else if (vec == fvec3(0,-1,0)) {
        return BlockOrientation::BOTTOM;
    }
    return BlockOrientation::FRONT;
}*/
