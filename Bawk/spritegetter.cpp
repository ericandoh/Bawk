//
//  spritegetter.cpp
//  Bawk
//
//  Created by Eric Oh on 12/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spritegetter.h"

#include "spriterender.h"

#include "json/json.h"
#include "json_reader_helper.h"
#include "game_info_loader.h"

#include "spriteexplosion.h"
#include "spritemodel.h"

SpriteMultiplexer* get_sprite_multiplexer_from_string(std::string name, Json::Value node) {
    if (name.compare("explosion") == 0) {
        return get_sprite_explosive(node);
    }
    else if (name.compare("model") == 0) {
        return get_sprite_model(node);
    }
    return 0;
}

// if we add more properties, make fetching methods for them here

// called by game, and I guess by things that want to make sprites?
SpriteRender* get_sprite_instance(int sid) {
    // get sprite from gameinfoloader
    SpriteRender* base_sprite = get_sprite_renderable(sid);
    // copy the sprite, since it needs its own state/whatnot
    SpriteRender* copy = base_sprite->copy();
    return copy;
}

// Create a SpriteRender instance from the description in JSON
// For example, if the JSON was formatted like:
//
//   [{"render": "explosive", "texture": 3}, {"step": "follow", "who": "closest"}]
//
// we would add the properties such that our sprite renders an explosion of texture 3
// and so that our sprite, on each step, follows the closest entity (or whatever that
// behaviour we define as doing
void set_sprite_properties(SpriteRender* render, Json::Value node) {
    if (!node.isArray()) {
        return;
    }
    int properties_size = node.size();
    for (int i = 0; i < properties_size; i++) {
        Json::Value property = node[i];
        if (!property.isObject()) {
            continue;
        }
        if (property.isMember("id")) {
            render->multiplexer = get_sprite_multiplexer_from_string(json_read_string_safe(property, "id"), property);
        }
    }
}