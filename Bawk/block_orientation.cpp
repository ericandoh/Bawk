//
//  block_orientation.cpp
//  Bawk
//
//  Created by Eric Oh on 11/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "block_orientation.h"
#include <glm/gtc/matrix_transform.hpp>
#include "rotation.h"

struct OrientationMath {
    Rotation orientation_rotation[BLOCK_ORIENTATION_COUNT];
    BlockOrientation into_my_rotation[BLOCK_ORIENTATION_COUNT][BLOCK_ORIENTATION_COUNT];
};

OrientationMath orientation_math_vars;

void init_orientation_rotation_calculations() {
    for (int i = 0; i < BLOCK_ORIENTATION_COUNT; i++) {
        BlockOrientation i_ori = static_cast<BlockOrientation>(i);
        orientation_math_vars.orientation_rotation[i] = get_rotation_from_orientation(i_ori);
        for (int j = 0; j < BLOCK_ORIENTATION_COUNT; j++) {
            BlockOrientation j_ori = static_cast<BlockOrientation>(j);
            Rotation j_rot = get_rotation_from_orientation(j_ori);
            // given world orientation j_ori find k_ori in my rotation
            Rotation dst;
            orientation_math_vars.orientation_rotation[i].transform_into_my_rotation(&dst, j_rot);
            orientation_math_vars.into_my_rotation[i][j] = get_orientation_from_rotation(dst);
        }
    }
}

fvec3 get_direction_from_orientation(BlockOrientation a) {
    switch (a) {
        case BlockOrientation::FRONT:
            return fvec3(1,0,0);
        case BlockOrientation::TOP:
            return fvec3(0,1,0);
        case BlockOrientation::LEFT:
            return fvec3(0,0,1);
        case BlockOrientation::BACK:
            return fvec3(-1,0,0);
        case BlockOrientation::BOTTOM:
            return fvec3(0,-1,0);
        case BlockOrientation::RIGHT:
            return fvec3(0,0,-1);
        default:
            return fvec3(0,0,0);
    }
}

BlockOrientation get_orientation_from_direction(fvec3 dir) {
    float xabs = fabsf(dir.x);
    float yabs = fabsf(dir.y);
    float zabs = fabsl(dir.z);
    if (xabs < zabs) {
        if (yabs < zabs) {
            // x < z, y < z
            // pointing in z-direction
            if (dir.z < 0) {
                return BlockOrientation::RIGHT;
            }
            else {
                return BlockOrientation::LEFT;
            }
        }
        else {
            // x < z < y
            // pointing in y-direction
            if (dir.y < 0) {
                return BlockOrientation::BOTTOM;
            }
            else {
                return BlockOrientation::TOP;
            }
        }
    } else if (xabs < yabs) {
        // z < x < y
        // pointing in y-direction
        if (dir.y < 0) {
            return BlockOrientation::BOTTOM;
        }
        else {
            return BlockOrientation::TOP;
        }
    } else {
        // z < x, y < x
        // pointing in x-direction
        if (dir.x < 0) {
            return BlockOrientation::BACK;
        }
        else {
            return BlockOrientation::FRONT;
        }
    }
}

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

/*
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
};*/

// used for setting individual faces of blocks relative to block local orientation
// base: orientation of the block (where FRONT in block aligned space points in real space)
// dir:  orientation in real space
// RETURN: orientation in block aligned space
BlockOrientation get_translated_orientation(BlockOrientation base, BlockOrientation dir) {
    return orientation_math_vars.into_my_rotation[base][dir];
    //return relative_orientations[base][dir];
}

BlockOrientation get_orientation_from_rotation(Rotation rot) {
    fvec3 dir = rot.get_dir();
    return get_orientation_from_direction(dir);
}

Rotation get_rotation_from_orientation(BlockOrientation a) {
    fvec3 dir, up;
    if (a == BlockOrientation::TOP) {
        up = fvec3(-1,0,0);
    }
    else if (a == BlockOrientation::BOTTOM) {
        up = fvec3(1,0,0);
    }
    else {
        up = fvec3(0,1,0);
    }
    dir = get_direction_from_orientation(a);
    Rotation result = Rotation();
    result.set_to_point(dir, up);
    return result;
}

/*
void test_orientation_math() {
    for (int i = 0; i < BLOCK_ORIENTATION_COUNT; i++) {
        fvec4 val(1,0,0,0);
        fmat4 one(1);
        orientation_math_vars.orientation_rotation[i].add_world_rotation(&one);
        fvec4 result = one * val;
        fvec3 dir_result = fvec3(result.x, result.y, result.z);
        BlockOrientation ori = get_orientation_from_direction(dir_result);
        if ((int)ori != i) {
            printf("-->rotation math fail (%d):\n", i);
            printf_fvec3(dir_result);
            printf("\n%d != %d\n", ori, i);
        }
        for (int j = 0; j < BLOCK_ORIENTATION_COUNT; j++) {
            BlockOrientation hand_result = relative_orientations[i][j];
            BlockOrientation auto_result = orientation_math_vars.into_my_rotation[i][j];
            if (hand_result != auto_result) {
                printf("-->into rotation fail (%d,%d):\n", i, j);
                printf("%d != %d\n", hand_result, auto_result);
            }
        }
    }
}
*/
