//
//  basic_types.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "basic_types.h"
#include <algorithm>
#include <string>

uint32_t spread_bits(uint32_t val)
{
    uint32_t result = 0x0;
    for (int i = 0; i < 9; i++) {
        result += ((0x1 << i) & val) << (i*2);
    }
    return result;
}

int imin(int a, int b) {
    return a < b ? a : b;
}

int imax(int a, int b) {
    return a > b ? a : b;
}

ivec3 get_floor_from_fvec3(fvec3 src) {
    ivec3 result((int)floorf(src.x), (int)floorf(src.y), (int)floorf(src.z));
    return result;
}

fvec3 get_round_from_fvec3(fvec3 src) {
    return fvec3(roundf(src.x), roundf(src.y), roundf(src.z));
}

ivec3 get_ivec3_minimum(ivec3 a, ivec3 b) {
    ivec3 result;
    result.x = std::min(a.x, b.x);
    result.y = std::min(a.y, b.y);
    result.z = std::min(a.z, b.z);
    return result;
}

ivec3 get_ivec3_maximum(ivec3 a, ivec3 b) {
    ivec3 result;
    result.x = std::max(a.x, b.x);
    result.y = std::max(a.y, b.y);
    result.z = std::max(a.z, b.z);
    return result;
}

bounding_box get_bounding_box_intersection(bounding_box a, bounding_box b) {
    bounding_box result;
    result.lower.x = std::max(a.lower.x, b.lower.x);
    result.lower.y = std::max(a.lower.y, b.lower.y);
    result.lower.z = std::max(a.lower.z, b.lower.z);
    
    result.upper.x = std::min(a.upper.x, b.upper.x);
    result.upper.y = std::min(a.upper.y, b.upper.y);
    result.upper.z = std::min(a.upper.z, b.upper.z);
    
    return result;
}


void printf_fvec3(fvec3 a) {
    printf("%f %f %f", a.x, a.y, a.z);
}

void printf_fvec4(fvec4 a) {
    printf("%f %f %f %f", a.x, a.y, a.z, a.w);
}

int is_closer_to_half(float a) {
    if ((int)roundf(a * 2.0f) % 2 == 1) {
        return 1;
    }
    return 0;
}

fvec3 get_nearest_half_or_whole(fvec3 a) {
    int to_half = 0;
    to_half += is_closer_to_half(a.x);
    to_half += is_closer_to_half(a.y);
    to_half += is_closer_to_half(a.z);
    
    if (to_half > 1) {
        // 2 or 3 closer to half
        // round to nearest 0.5
        fvec3 result(roundf(a.x - 0.5f)+0.5f,
                     roundf(a.y - 0.5f)+0.5f,
                     roundf(a.z - 0.5f)+0.5f);
        return result;
    }
    else {
        // round to nearest 1.0
        fvec3 result(roundf(a.x),
                     roundf(a.y),
                     roundf(a.z));
        return result;
    }
}
