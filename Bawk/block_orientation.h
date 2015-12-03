//
//  block_orientation.h
//  Bawk
//
//  Created by Eric Oh on 11/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__block_orientation__
#define __Bawk__block_orientation__

#include <stdio.h>
#include <stdint.h>
#include "basic_types.h"

class Rotation;

// orientation, from a viewer looking from the positive x-direction
enum BlockOrientation: uint8_t {
    FRONT = 0,
    TOP = 1,
    LEFT = 2,
    BACK = 3,
    BOTTOM = 4,
    RIGHT = 5
};

const int BLOCK_ORIENTATION_COUNT = 6;
const BlockOrientation DEFAULT_BLOCK_ORIENTATION = BlockOrientation::FRONT;

void init_orientation_rotation_calculations();

fvec3 get_direction_from_orientation(BlockOrientation a);
BlockOrientation get_orientation_from_direction(fvec3 dir);

// used for moving block relative to player
// used for setting orientation of block when placed
BlockOrientation get_nearest_compass_direction(fvec3 dir);

// used for knowing which face to render given a block and its orientation
BlockOrientation get_translated_orientation(BlockOrientation base, BlockOrientation dir);

BlockOrientation get_orientation_from_rotation(Rotation rot);
Rotation get_rotation_from_orientation(BlockOrientation a);

//void test_orientation_math();

#endif /* defined(__Bawk__block_orientation__) */
