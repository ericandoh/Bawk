//
//  cursorselector.cpp
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "cursorselector.h"
#include "blocktracer.h"
#include "superobject.h"

// methods to initialize/cleanup this entity
void CursorSelector::init() {
    is_selected = false;
    is_block_selected = false;
}

void CursorSelector::reset() {
    is_selected = false;
    is_block_selected = false;
}

// behaviour when this cursor item is clicked
bool CursorSelector::clicked(Game* game, Action mouse) {
    if (!BlockTracing::show_item) {
        is_selected = false;
        return true;
    }
    selected = BlockTracing::selected;
    if (selected->entity_class == EntityType::SUPEROBJECT ||
        selected->entity_class == EntityType::BASEWORLD ||
        selected->entity_class == EntityType::GAMETEMPLATE) {
        is_block_selected = true;
        selected_pos = BlockTracing::pointed_pos;
    }
    else {
        is_block_selected = false;
    }
    selected = selected->find_top_level_parent();
    if (selected->entity_class == EntityType::SUPEROBJECT) {
        ((SuperObject*)selected)->selected = true;
    }
    is_selected = true;
    return true;
}

bool CursorSelector::pushed(Game* game, Action key) {
    if (key == Action::DELETE) {
        if (is_selected && !is_block_selected) {
            selected->remove_self();
        }
    }
    return true;
}

// behaviour when this cursor item is entered
bool CursorSelector::confirmed(Game* game) {
    if (is_selected && !is_block_selected) {
        // TODO show info about the selected
    }
    return true;
}

bool CursorSelector::canceled(Game* game) {
    reset();
    return true;
}

// render the item with transform in a small box or whatnot
void CursorSelector::render_item() {
    
}
// render the item in the world
void CursorSelector::render_in_world(fmat4* transform) {
    
}

cursor_item_identifier CursorSelector::get_identifier() {
    return cursor_item_identifier();
}