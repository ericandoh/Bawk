//
//  basic_types.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "basic_types.h"
#include <algorithm>

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

ivec3 get_ivec3_minimum(ivec3 a, ivec3 b) {
    ivec3 result;
    result.x = std::min(a.x, b.x);
    result.y = std::min(a.y, b.y);
    result.z = std::min(a.z, b.z);
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

// base is where our new front is
// original is what we want our new orientation to be
// we will (for now) restrict ourselves to rotation in the xz plane only
// rotation in the y plane is for upside down/on its side which tbh is kinda weird (wall plants anyone?)
// we can support rotation in the y-plane later, as a future // TOFU
BlockOrientation get_translated_orientation(BlockOrientation base, BlockOrientation original) {
    if (original == BlockOrientation::TOP || original == BlockOrientation::BOTTOM) {
        return original;
    }
    
    if (base == BlockOrientation::TOP) {
        base = BlockOrientation::FRONT;
    }
    if (base == BlockOrientation::BOTTOM) {
        base = BlockOrientation::BACK;
    }
    
    if (base == BlockOrientation::FRONT) {
        return original;
    }
    else if (base == BlockOrientation::BACK) {
        // flip left to right, front to back
        if (base < 3) {
            return static_cast<BlockOrientation>(base + 3);
        }
        return static_cast<BlockOrientation>(base - 3);
    }
    else if (base == BlockOrientation::RIGHT) {
        if (original == BlockOrientation::RIGHT) {
            return BlockOrientation::FRONT;
        }
        else if (original == BlockOrientation::FRONT) {
            return BlockOrientation::LEFT;
        }
        else if (original == BlockOrientation::LEFT) {
            return BlockOrientation::BACK;
        }
        else if (original == BlockOrientation::BACK) {
            return BlockOrientation::RIGHT;
        }
    }
    else if (base == BlockOrientation::LEFT) {
        if (original == BlockOrientation::LEFT) {
            return BlockOrientation::FRONT;
        }
        else if (original == BlockOrientation::FRONT) {
            return BlockOrientation::RIGHT;
        }
        else if (original == BlockOrientation::RIGHT) {
            return BlockOrientation::BACK;
        }
        else if (original == BlockOrientation::BACK) {
            return BlockOrientation::LEFT;
        }
    }
    return BlockOrientation::FRONT;
}

ivec3 get_translated_offset(BlockOrientation base, ivec3 original_offset) {
    if (base == BlockOrientation::TOP) {
        base = BlockOrientation::FRONT;
    }
    if (base == BlockOrientation::BOTTOM) {
        base = BlockOrientation::BACK;
    }
    
    if (base == BlockOrientation::FRONT) {
        return original_offset;
    }
    else if (base == BlockOrientation::BACK) {
        // flip x, z
        ivec3 result = ivec3(-original_offset.x,
                             original_offset.y,
                             -original_offset.z);
        return result;
    }
    else if (base == BlockOrientation::RIGHT) {
        ivec3 result = ivec3(-original_offset.z,
                             original_offset.y,
                             original_offset.x);
        return result;
    }
    else if (base == BlockOrientation::LEFT) {
        ivec3 result = ivec3(original_offset.z,
                             original_offset.y,
                             -original_offset.x);
        return result;
    }
    return original_offset;
}