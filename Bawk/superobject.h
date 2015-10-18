//
//  A superobject represents an entity of blocks that will be saved/loaded ingame
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
    uint32_t vid;
    uint32_t pid;
protected:
    std::string world_name;
public:
    SuperObject(std::string w);
    SuperObject(std::string w, uint32_t p, uint32_t v);
    SuperObject(std::string w, uint32_t p, uint32_t v, int* err);
    
    virtual std::string get_save_path() override;
    
    virtual int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    virtual int save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) override;
    virtual bool within_dimensions_chunk(int x, int y, int z) override;
};

#endif /* defined(__Bawk__Superobject__) */
