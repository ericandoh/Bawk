//
//  Superobject.h
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__Superobject__
#define __Bawk__Superobject__

#include <stdio.h>
#include <string>
#include "superobjectrender.h"

class SuperObject : public RenderableSuperObject {
    std::string name;
    std::string world_name;
public:
    SuperObject(std::string wid);
    SuperObject(std::string id, std::string wid);
    int get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) override;
    int save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) override;
};

#endif /* defined(__Bawk__Superobject__) */
