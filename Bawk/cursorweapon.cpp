//
//  cursorweapon.cpp
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "cursorweapon.h"
#include "blocktracer.h"
#include "game.h"

CursorWeapon::CursorWeapon(CursorItemInfo* i): CursorItem(i) {
    dmg = 500.0f;
}

bool CursorWeapon::clicked(Game* game, Action mouse) {
    return true;
}

bool CursorWeapon::clicking(Game* game, Action mouse, int ms) {
    if (!BlockTracing::show_item)
        return false;
    Entity* src = BlockTracing::selected;
    if (!src)
        return false;
    
    // get the location in the object we want to hit
    src->get_hurt(BlockTracing::pointed_pos.x,
                  BlockTracing::pointed_pos.y,
                  BlockTracing::pointed_pos.z,
                  dmg, BreakablePolicy::ACTIONED, game->player);
    return true;
}