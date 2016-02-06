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
#include "game.h"
#include "gametemplate.h"

CursorItem::CursorItem(CursorItemInfo* i) {
    info = i;
    target = 0;
    show_item = false;
    pointing_pos = ivec3(0,0,0);
}

void CursorItem::update_pointing_position(Game* game, ivec3 dimensions) {
    if (get_pointing_position(&pointing_pos, &pointing_orientation, dimensions)) {
        show_item = true;
        if (game->game_template) {
            target = game->game_template;
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

// behaviour when this cursor item is clicked
bool CursorItem::clicked(Game* game, Action mouse) {
    return false;
}

// behaviour when this cursor item is entered
bool CursorItem::confirmed(Game* game) {
    return false;
}

bool CursorItem::canceled(Game* game) {
    return false;
}

bool CursorItem::handle_movement(ivec3 dir) {
    return false;
}

bool CursorItem::handle_rotation() {
    return false;
}

int CursorItem::get_count() {
    return info->count;
}