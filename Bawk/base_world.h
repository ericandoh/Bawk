//
//  The world, excluding vehicles. Only includes world blocks
//  - Is NOT the whole world. For the entire world see world.h
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
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__base_world__
#define __Bawk__base_world__

#include <stdio.h>
#include <string>
#include "superobject.h"

class BaseWorld: public SuperObject {
    block_type air[CX][CY][CZ];
    block_type ground[CX][CY][CZ];
public:
    BaseWorld(std::string wid);
    std::string get_save_path() override;
    std::string get_chunk_save_path(ivec3* pos) override;
    int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    //int save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) override;
    bool within_dimensions_chunk(int x, int y, int z) override;
    void update_dimensions_from_chunk(ivec3 chunk_pos) override;
    bool intersects_with_my_bounds(fvec3 lower_corner, fvec3 upper_corner) override;
    
    bool poke(float x, float y, float z) override;
    int get_collision_priority() override;
};

#endif /* defined(__Bawk__base_world__) */
