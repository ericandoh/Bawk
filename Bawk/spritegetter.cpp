//
//  spritegetter.cpp
//  Bawk
//
//  Created by Eric Oh on 12/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spritegetter.h"

#include "spriterender.h"
#include "spriteexplosion.h"
#include "json/json.h"
#include "json_reader_helper.h"
#include "game_info_loader.h"

SpriteRenderable* get_sprite_renderable_from_string(std::string name, Json::Value node) {
    if (name.compare("explosion") == 0) {
        // read stuff from node
        int texture_id;
        float start_radius, end_radius;
        texture_id = start_radius = end_radius = 0;
        std::string key = "texture";
        json_read_int_safe(&texture_id, node, "texture");
        json_read_float_safe(&start_radius, node, "start_radius");
        json_read_float_safe(&end_radius, node, "end_radius");
        return get_sprite_explosive(texture_id, start_radius, end_radius);
    }
    return 0;
}

SpriteSteppable* get_sprite_steppable_from_string(std::string name, Json::Value node) {
    return 0;
}

// if we add more properties, make fetching methods for them here

// called by game, and I guess by things that want to make sprites?
SpriteRender* get_sprite_instance(int sid, Player* p) {
    // get sprite from gameinfoloader
    SpriteRender* base_sprite = get_sprite_renderable(sid);
    // copy the sprite, since it needs its own state/whatnot
    SpriteRender* copy = base_sprite->copy();
    copy->player = p;
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
        if (property.isMember("render")) {
            render->renderable = get_sprite_renderable_from_string(json_read_string_safe(property, "render"), property);
        }
        if (property.isMember("step")) {
            render->steppable = get_sprite_steppable_from_string(json_read_string_safe(property, "step"), property);
        }
        // if we add more properties add them here
    }
}