//
//  basic_types.h
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_basic_types_h
#define Bawk_basic_types_h

#include "glm/glm.hpp"

// float vec3
typedef glm::vec3 fvec3;

// float vec2
typedef glm::vec2 fvec2;

// INPUT: ABC
// OUTPUT: 00A00B00C
uint32_t spread_bits(uint32_t val);

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
    /*
    // return hash function based on interleaving of x, y, z
    std::size_t operator()(const ivec3& k) const
    {
        // turn first 9 bits to a 27 bit string, then add together
        // the result should be XYZXYZXYZXYZ for XXXX, YYYY, ZZZZ (but with more bits)
        uint32_t xb = spread_bits(x);
        uint32_t yb = spread_bits(y) << 1;
        uint32_t zb = spread_bits(z) << 2;
        return xb + yb + zb;
    }*/
};

// 4x4 matrix, for rendering calculations
typedef glm::mat4 fmat4;

// float

#endif
