//
//  blockaction.cpp
//  Bawk
//
//  Created by Eric Oh on 10/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "importopengl.h"
#include "blockaction.h"
#include "game.h"
#include "entity.h"

#include "modelactionmount.h"
#include "modelactionmovement.h"
#include "modelactionshoot.h"
#include "modelactionexplode.h"

// ----- ALL BLOCK ACTIONS -----
bool block_default_callback(Game* game, Entity* owner, block_type* blk, ivec3 rwc) {
    return true;
}

// ----- BLOCK FETCH METHODS -----
void get_block_mouse_callback_for(std::string name, std::map<Action, block_callback_func> &action_map) {
    //if (name.compare("engine") == 0) {
        //action_map[Action::TURN_SEXY] = block_turn_sexy_callback
    //}
}

void get_block_keyboard_callback_for(std::string name, std::map<Action, block_callback_func> &action_map) {
    //if (name.compare("engine") == 0) {
    //action_map[Action::DANCE_SEXY] = block_dance_sexy_callback
    //}
}
