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
#include <map>
#include <unordered_map>
#include "superobjectrender.h"

struct key_mapping_info {
    ivec3 position;
    block_type blk;
    int action;
};

class SuperObject : public RenderableSuperObject {
protected:
    std::string world_name;
    // note that we might have >1 action per key press (ie. both engines X, Y fire on pressing W)
    std::map<int, std::vector<key_mapping_info>> key_mapping;
    // the reverse mapping of the above, for convenience
    std::unordered_map<ivec3, std::vector<int>> reverse_key_mapping;
public:
    uint32_t vid;
    uint32_t pid;
    SuperObject(std::string w);
    SuperObject(uint32_t p, uint32_t v);
    SuperObject(std::string w, uint32_t p, uint32_t v, ivec3 po);
    SuperObject(std::string w, uint32_t p, uint32_t v, int* err);
    
    void set_block(float x, float y, float z, block_type type) override;
    
    bool block_keyboard_callback(Game* game, int key) override;
    bool block_mouse_callback(Game* game, int button) override;
    
    virtual std::string get_save_path() override;
    virtual std::string get_chunk_save_path(ivec3* pos);
    
    virtual int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    virtual int save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) override;
    
    int load_self(IODataObject* obj) override;
    void remove_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__Superobject__) */
