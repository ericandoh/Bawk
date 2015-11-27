//
//  cursoritem.cpp
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursoritem.h"
#include "entity.h"

// methods to initialize/cleanup this entity
void CursorItem::init() { }

void CursorItem::cleanup() { }

void CursorItem::reset() { }

// behaviour when this cursor item is clicked
bool CursorItem::clicked(Game* game, Action mouse, Entity* on) {
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