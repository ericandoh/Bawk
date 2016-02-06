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

CursorSelector::CursorSelector(CursorItemInfo* i): CursorItem(i) {
    selected = 0;
    is_selected = false;
    is_block_selected = false;
}

// methods to initialize/cleanup this entity
void CursorSelector::init() {
    unselect();
}

void CursorSelector::reset() {
    unselect();
}

void CursorSelector::unselect() {
    if (selected) {
        selected->selected = false;
    }
    is_selected = false;
    is_block_selected = false;
    selected = 0;
}

void CursorSelector::select(Entity* ent) {
    unselect();
    selected = ent;
    if (ent->entity_class != EntityType::BASEWORLD) {
        ent->selected = true;
    }
    is_selected = true;
}

// behaviour when this cursor item is clicked
bool CursorSelector::clicked(Game* game, Action mouse) {
    if (!BlockTracing::show_item) {
        unselect();
        return true;
    }
    Entity* ent = BlockTracing::selected;
    if (ent->entity_class == EntityType::SUPEROBJECT ||
        ent->entity_class == EntityType::BASEWORLD ||
        ent->entity_class == EntityType::GAMETEMPLATE) {
        is_block_selected = true;
        selected_pos = BlockTracing::pointed_pos;
    }
    else {
        is_block_selected = false;
    }
    ent = ent->find_top_level_parent();
    select(ent);
    return true;
}

bool CursorSelector::pushed(Game* game, Action key) {
    if (key == Action::DELETE) {
        if (is_selected && !is_block_selected && selected->entity_class != EntityType::BASEWORLD) {
            Entity* deselect = selected;
            unselect();
            deselect->remove_self();
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
    if (is_selected) {
        unselect();
        return true;
    }
    return false;
}

// render the item with transform in a small box or whatnot
void CursorSelector::render_item() {
    
}
// render the item in the world
void CursorSelector::render_in_world(fmat4* transform) {
    
}
