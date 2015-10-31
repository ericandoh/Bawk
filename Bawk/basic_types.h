//
//  Holds basic types/typedefs used in the game
//  -
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
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_basic_types_h
#define Bawk_basic_types_h

#include "glm/glm.hpp"
#include <climits>
#include <algorithm>

// float vec3
typedef glm::vec3 fvec3;

// float vec2
typedef glm::vec2 fvec2;

typedef glm::vec4 fvec4;

// float vec4
typedef glm::tvec4<int> ivec4;

struct fvec6 {
    float a, b, c, d, e, f;
};

// INPUT: ABC
// OUTPUT: 00A00B00C
uint32_t spread_bits(uint32_t val);

int imin(int a, int b);
int imax(int a, int b);

// a vec3 of ints that can be hashed (with coordinate locality)
struct ivec3 : glm::tvec3<int> {
    ivec3(int ix=0, int iy=0, int iz=0) {
        x = ix; y = iy; z = iz;
    }
    /*bool operator<(const ivec3& b) const
    {
        return hash_value() < b.hash_value();
    };*/
    // added functions so that we can use these as keys in a map!
    bool operator==(const ivec3 &other) const
    {
        return (x == other.x
              && y == other.y
              && z == other.z);
    }
};

// orientation, from a viewer looking from the positive x-direction
enum BlockOrientation: uint8_t {
    FRONT = 0,
    TOP = 1,
    RIGHT = 2,
    BACK = 3,
    BOTTOM = 4,
    LEFT = 5
};

struct player_and_id {
    uint32_t pid;
    uint32_t sid;
    bool operator==(const player_and_id &other) const
    {
        return (pid == other.pid
                && sid == other.sid);
    }
};

struct cursor_item_distinguisher {
    bool is_blk;
    bool is_recipe;
    uint16_t bid;
    uint32_t pid;
    uint32_t vid;
};

enum SaveMode {
    READ, WRITE,
};

// 4x4 matrix, for rendering calculations
typedef glm::mat4 fmat4;

struct bounding_box {
    fvec3 lower;
    fvec3 upper;
    bounding_box() {
        lower = fvec3(FLT_MAX, FLT_MAX, FLT_MAX);
        upper = fvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }
    bounding_box(fvec3 l) {
        lower = l;
        upper = fvec3(lower.x + 1,
                      lower.y + 1,
                      lower.z + 1);
    }
    bounding_box(fvec3 l, fvec3 u) {
        lower = l;
        upper = u;
    }
    bool hits(fvec3 point) {
        return (lower.x <= point.x) && (point.x <= upper.x) &&
        (lower.y <= point.y) && (point.y <= upper.y) &&
        (lower.z <= point.z) && (point.z <= upper.z);
    }
    bool hits(bounding_box other) {
        return !(lower.x >= other.upper.x || other.lower.x >= upper.x
          || lower.y >= other.upper.y || other.lower.y >= upper.y
          || lower.z >= other.upper.z || other.lower.z >= upper.z);
    }
    void refit_for_rotation() {
        float lowerx = std::min(lower.x, upper.x);
        float lowery = std::min(lower.y, upper.y);
        float lowerz = std::min(lower.z, upper.z);
        float upperx = std::max(lower.x, upper.x);
        float uppery = std::max(lower.y, upper.y);
        float upperz = std::max(lower.z, upper.z);
        lower.x = lowerx;
        lower.y = lowery;
        lower.z = lowerz;
        upper.x = upperx;
        upper.y = uppery;
        upper.z = upperz;
    }
};

bounding_box get_bounding_box_intersection(bounding_box a, bounding_box b);

ivec3 get_floor_from_fvec3(fvec3 src);
ivec3 get_ivec3_minimum(ivec3 a, ivec3 b);

BlockOrientation get_translated_orientation(BlockOrientation base, BlockOrientation original);
ivec3 get_translated_offset(BlockOrientation base, ivec3 original_offset);

#endif
