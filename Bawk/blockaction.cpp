//
//  blockaction.cpp
//  Bawk
//
//  Created by Eric Oh on 10/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "includeglfw.h"
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

// ----- ALL MODEL ACTIONS -----
bool model_default_callback(Game* game, Entity* owner, Entity* piece) {
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

// ----- MODEL FETCH METHODS -----

void get_model_mouse_callback_for(std::string name, std::map<Action, model_callback_func> &action_map) {
    if (name.compare("vehicle") == 0) {
        action_map[Action::CLICK_CREATE] = model_mount_callback;
    }
}

void get_model_keyboard_callback_for(std::string name, std::map<Action, model_callback_func> &action_map) {
    if (name.compare("engine") == 0) {
        action_map[Action::MOVE_UP] = model_up_callback;
        action_map[Action::MOVE_DOWN] = model_down_callback;
        action_map[Action::MOVE_LEFT] = model_turn_left_callback;
        action_map[Action::MOVE_RIGHT] = model_turn_right_callback;
        action_map[Action::MOVE_FORWARD] = model_forward_callback;
        action_map[Action::MOVE_BACKWARD] = model_backward_callback;
        action_map[Action::PITCH_UP] = model_pitch_up_callback;
        action_map[Action::PITCH_DOWN] = model_pitch_down_callback;
        action_map[Action::ROLL_LEFT] = model_roll_left_callback;
        action_map[Action::ROLL_RIGHT] = model_roll_right_callback;
    }
    else if (name.compare("vehicle") == 0) {
        action_map[Action::MOUNTING] = model_unmount_callback;
    }
    else if (name.compare("shoot") == 0) {
        action_map[Action::CLICK_DESTROY] = model_shoot_callback;
    }
    else if (name.compare("explode") == 0) {
        action_map[Action::COLLIDE] = model_explode_callback;
    }
}
