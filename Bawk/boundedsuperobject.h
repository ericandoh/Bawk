//
//  boundedsuperobject.h
//  Bawk
//
//  Created by Eric Oh on 10/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__boundedsuperobject__
#define __Bawk__boundedsuperobject__

#include <stdio.h>
#include "superobject.h"

class BoundedSuperObject: public SuperObject {
    // bounds of the object
    ivec3 lower_bound;
    ivec3 upper_bound;
    int id;
public:
    BoundedSuperObject(std::string id, std::string wid);
    int get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) override;
    int save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) override;
    bool within_dimensions(int x, int y, int z) override;
    void update_dimensions(ivec3* pos) override;
};

#endif /* defined(__Bawk__boundedsuperobject__) */
