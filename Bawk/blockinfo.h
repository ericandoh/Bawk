//
//  blockinfo.h
//  Bawk
//
//  Created by Eric Oh on 12/8/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__blockinfo__
#define __Bawk__blockinfo__

#include <stdio.h>
#include <string>
#include <map>
#include "lightrender.h"
#include "blockaction.h"

class BlockInfo {
public:
    std::string name;
    bool is_model;
    int texture;
    int textures[6];
    int resistance;
    int transparency;
    int weight;
    int vehicle;
    RenderableLight light;
    
    bool has_mouse_callback;
    bool has_keyboard_callback;
    std::map<Action, block_callback_func> mouse_callbacks;
    std::map<Action, block_callback_func> keyboard_callbacks;
    
    BlockInfo();
    void set_callback_checks();
    
    // --- accessor methods ---
    uint16_t get_block_texture(BlockOrientation orientation, BlockOrientation face);
};

#endif /* defined(__Bawk__blockinfo__) */
