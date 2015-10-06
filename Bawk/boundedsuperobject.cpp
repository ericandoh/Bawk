//
//  boundedsuperobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "boundedsuperobject.h"

static int object_counter = 0;

BoundedSuperObject::BoundedSuperObject(std::string id, std::string wid): SuperObject(id, wid) {
    // initialize fields
    id = object_counter++;
}

// TODO (override set method to update bounds)

int BoundedSuperObject::get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) {
    if (!within_dimensions(x, y, z)) {
        // not within dimensions, unable to fetch
        return -1;
    }
    return SuperObject::get_chunk(to_arr, x, y, z);
}

int BoundedSuperObject::save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) {
    // TODO update the bounds object in the special disk data file
    return SuperObject::save_chunk(from_arr, x, y, z);
}

bool BoundedSuperObject::within_dimensions(int x, int y, int z) {
    return x >= lower_bound.x && y >= lower_bound.y && z >= lower_bound.z &&
            x <= upper_bound.x && y <= upper_bound.y && z <= upper_bound.z;
}

void BoundedSuperObject::update_dimensions(ivec3* pos) {
    lower_bound.x = minimum(lower_bound.x, pos->x);
    lower_bound.y = minimum(lower_bound.y, pos->y);
    lower_bound.z = minimum(lower_bound.z, pos->z);
    
    upper_bound.x = maximum(upper_bound.x, pos->x);
    upper_bound.y = maximum(upper_bound.y, pos->y);
    upper_bound.z = maximum(upper_bound.z, pos->z);
}