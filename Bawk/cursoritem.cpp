//
//  cursoritem.cpp
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursoritem.h"
#include "entity.h"
#include "blocktracer.h"
#include "gametemplate.h"
#include "client_accessor.h"

CursorItem::CursorItem(CursorItemInfo* i) {
    info = i;
    target = 0;
    show_item = false;
    pointing_pos = ivec3(0,0,0);
}

void CursorItem::update_pointing_position(ivec3 dimensions) {
    if (get_pointing_position(&pointing_pos, &pointing_orientation, dimensions)) {
        show_item = true;
        GameTemplate* gtemplate = get_player()->game_template;
        if (gtemplate) {
            target = gtemplate;
        }
        else {
            Entity* at = BlockTracing::selected;
            if (at->entity_class == EntityType::BASEWORLD ||
                at->entity_class == EntityType::SUPEROBJECT ||
                at->entity_class == EntityType::GAMETEMPLATE) {
                target = (SuperObject*)at;
            }
            else {
                target = at->parent;
            }
        }
    }
    else {
        show_item = false;
        target = 0;
    }
}

// methods to initialize/cleanup this entity

void CursorItem::cleanup() {
    delete this;
}

int CursorItem::get_count() {
    return info->count;
}