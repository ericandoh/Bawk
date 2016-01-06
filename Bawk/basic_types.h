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

#define EMPTY_BOOL_FUNCTION { return false; }
#define EMPTY_FUNCTION { }


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

namespace std
{
    template<>
    struct hash<ivec3> {
        size_t operator()(const ivec3 &pt) const {
            // turn first 9 bits to a 27 bit string, then add together
            // the result should be XYZXYZXYZXYZ for XXXX, YYYY, ZZZZ (but with more bits)
            uint32_t xb = spread_bits(pt.x);
            uint32_t yb = spread_bits(pt.y) << 1;
            uint32_t zb = spread_bits(pt.z) << 2;
            return xb + yb + zb;
        }
    };
}

struct player_and_id {
    uint32_t pid;
    uint32_t sid;
    bool operator==(const player_and_id &other) const
    {
        return (pid == other.pid
                && sid == other.sid);
    }
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
    void expand(fvec3 a) {
        lower.x = std::min(lower.x, a.x);
        lower.y = std::min(lower.y, a.y);
        lower.z = std::min(lower.z, a.z);
        upper.x = std::max(upper.x, a.x);
        upper.y = std::max(upper.y, a.y);
        upper.z = std::max(upper.z, a.z);
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
    void combine_with(bounding_box other) {
        lower.x = std::min(lower.x, other.lower.x);
        lower.y = std::min(lower.y, other.lower.y);
        lower.z = std::min(lower.z, other.lower.z);
        upper.x = std::max(upper.x, other.upper.x);
        upper.y = std::max(upper.y, other.upper.y);
        upper.z = std::max(upper.z, other.upper.z);
    }
    fvec3 get_center_pos() {
        fvec3 center_pos;
        center_pos.x = (lower.x + upper.x) / 2.0f;
        center_pos.y = (lower.y + upper.y) / 2.0f;
        center_pos.z = (lower.z + upper.z) / 2.0f;
        return center_pos;
    }
};

struct int_bounding_box {
    ivec3 lower, upper;
    int_bounding_box() {
        lower = ivec3(INT32_MAX, INT32_MAX, INT32_MAX);
        upper = ivec3(INT32_MIN, INT32_MIN, INT32_MIN);
    }
    int_bounding_box(ivec3 l, ivec3 u) {
        lower = l;
        upper = u;
    }
    void refit() {
        int lowerx = std::min(lower.x, upper.x);
        int lowery = std::min(lower.y, upper.y);
        int lowerz = std::min(lower.z, upper.z);
        int upperx = std::max(lower.x, upper.x);
        int uppery = std::max(lower.y, upper.y);
        int upperz = std::max(lower.z, upper.z);
        lower.x = lowerx;
        lower.y = lowery;
        lower.z = lowerz;
        upper.x = upperx;
        upper.y = uppery;
        upper.z = upperz;
    }
    fvec3 get_center_pos() {
        fvec3 center_pos;
        center_pos.x = (lower.x + upper.x) / 2.0f;
        center_pos.y = (lower.y + upper.y) / 2.0f;
        center_pos.z = (lower.z + upper.z) / 2.0f;
        return center_pos;
    }
    ivec3 get_dimensions() {
        ivec3 result(upper.x - lower.x,
                     upper.y - lower.y,
                     upper.z - lower.z);
        return result;
    }
    void expand(ivec3 a) {
        lower.x = std::min(lower.x, a.x);
        lower.y = std::min(lower.y, a.y);
        lower.z = std::min(lower.z, a.z);
        upper.x = std::max(upper.x, a.x);
        upper.y = std::max(upper.y, a.y);
        upper.z = std::max(upper.z, a.z);
    }
};

bounding_box get_bounding_box_intersection(bounding_box a, bounding_box b);

ivec3 get_floor_from_fvec3(fvec3 src);
fvec3 get_round_from_fvec3(fvec3 src);
ivec3 get_ivec3_minimum(ivec3 a, ivec3 b);
ivec3 get_ivec3_maximum(ivec3 a, ivec3 b);

float get_fvec3_distance(fvec3 src);

void printf_fvec3(fvec3 a);
void printf_fvec4(fvec4 a);

fvec3 get_nearest_half_or_whole(fvec3 a);

fvec3 get_random_color();

#endif
