//
//  basic_types.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "basic_types.h"

uint32_t spread_bits(uint32_t val)
{
    uint32_t result = 0x0;
    for (int i = 0; i < 9; i++) {
        result += ((0x1 << i) & val) << (i*2);
    }
    return result;
}

int minimum(int a, int b) {
    return a < b ? a : b;
}

int maximum(int a, int b) {
    return a > b ? a : b;
}