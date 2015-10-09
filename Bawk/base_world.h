//
//  base_world.h
//  Bawk
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
    uint16_t air[CX][CY][CZ];
    uint16_t ground[CX][CY][CZ];
public:
    BaseWorld(std::string wid);
    int get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) override;
    bool within_dimensions_chunk(int x, int y, int z);
};

#endif /* defined(__Bawk__base_world__) */
