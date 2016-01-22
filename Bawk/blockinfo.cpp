//
//  blockinfo.cpp
//  Bawk
//
//  Created by Eric Oh on 12/8/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "blockinfo.h"

BlockInfo::BlockInfo() {
    name = "";
    is_model = false;
    texture = 0;
    resistance = 1;
    transparency = 0;
    weight = 1;
    vehicle = 0;
    light = RenderableLight();
    
    has_mouse_callback = false;
    has_keyboard_callback = false;
}

void BlockInfo::set_callback_checks() {
    has_mouse_callback = !mouse_callbacks.empty();
    has_keyboard_callback = !keyboard_callbacks.empty();
}

uint16_t BlockInfo::get_block_texture(BlockOrientation orientation, BlockOrientation face) {
    if (texture < 0) {
        // use the array instead
        BlockOrientation actual_face = get_translated_orientation(orientation, face);
        return textures[actual_face];
    }
    else {
        return texture;
    }
    return 0;
}