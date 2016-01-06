//
//  cursordrill.cpp
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "cursordrill.h"
#include "blocktracer.h"
#include "game.h"

CursorDrill::CursorDrill() {
    dps = MAX_HEALTH / 3.0f;
}

bool CursorDrill::clicked(Game* game, Action mouse) {
    if (!BlockTracing::show_item)
        return false;
    Entity* src = BlockTracing::selected;
    if (!src)
        return false;
    // if (!src->aligned)
    // TODO remove this if we're not actually hitting the block at that position...
    //    return?
    
    if (src->entity_class == EntityType::PLAYER) {
        // can't hit myself...lol
        return false;
    }
    
    // get the location in the object we want to hit
    src->get_hurt(BlockTracing::pointed_pos.x,
                  BlockTracing::pointed_pos.y,
                  BlockTracing::pointed_pos.z,
                  500.0f, BreakablePolicy::EXECUTED, game->player->pid);
    return true;
}

void CursorDrill::render_item() {
    
}

void CursorDrill::render_in_world(fmat4 *transform) {
    
}

cursor_item_identifier CursorDrill::get_identifier() {
    return cursor_item_identifier();
}