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

// ----- ALL BLOCK ACTIONS -----
bool block_default_callback(Game* game, Entity* owner, block_type* blk, ivec3 rwc) {
    return true;
}

// --- MOUSE ---

// --- KEYBOARD ---

// ----- ALL MODEL ACTIONS -----
bool model_default_callback(Game* game, Entity* owner, Entity* piece) {
    return true;
}

// --- MOUSE ---
bool model_mount_callback(Game* game, Entity* owner, Entity* piece) {
    if (owner && owner->entity_class == EntityType::SUPEROBJECT) {
        fvec3 piece_pos = piece->pos + piece->center_pos + fvec3(0.0f, 0.75f, 0.0f);
        if (piece->parent)
            piece->parent->transform_into_world_coordinates(&piece_pos, piece_pos.x, piece_pos.y, piece_pos.z);
        piece_pos = piece_pos - game->player->center_pos;
        game->player->set_mount((SuperObject*)owner, piece_pos);
        return true;
    }
    return false;
}

// --- KEYBOARD ---
bool model_forward_callback(Game* game, Entity* owner, Entity* piece) {
    // piece->get_force() or some shizzle...and repeat for all below
    owner->move_forward(20.0f);
    return true;
}

bool model_backward_callback(Game* game, Entity* owner, Entity* piece) {
    owner->move_backward(5.0f);
    return true;
}

bool model_turn_left_callback(Game* game, Entity* owner, Entity* piece) {
    owner->yaw_left(4.0f);
    return true;
}

bool model_turn_right_callback(Game* game, Entity* owner, Entity* piece) {
    owner->yaw_right(4.0f);
    return true;
}

bool model_up_callback(Game* game, Entity* owner, Entity* piece) {
    owner->move_up(5.0f);
    return true;
}

bool model_down_callback(Game* game, Entity* owner, Entity* piece) {
    owner->move_down(5.0f);
    return true;
}

bool model_pitch_up_callback(Game* game, Entity* owner, Entity* piece) {
    owner->pitch_up(3.0f);
    return true;
}

bool model_pitch_down_callback(Game* game, Entity* owner, Entity* piece) {
    owner->pitch_down(3.0f);
    return true;
}

bool model_roll_left_callback(Game* game, Entity* owner, Entity* piece) {
    owner->roll_left(3.0f);
    return true;
}

bool model_roll_right_callback(Game* game, Entity* owner, Entity* piece) {
    owner->roll_right(3.0f);
    return true;
}

bool model_unmount_callback(Game* game, Entity* owner, Entity* piece) {
    game->player->unmount(game->world);
    return true;
}



// a universal 6 directional engine
// drawbacks is it is very weak
bool engine_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, ivec3 rwc, Action action) {
    // TODO we need both the owner entity (the engine on which this is called) and the target (the entire superobject
    // for which we want to do the action on...
    if (action == Action::MOVE_FORWARD) {
        // move forward
        owner->move_forward(20.0f);
    }
    else if (action == Action::MOVE_BACKWARD) {
        // move backwards
        owner->move_backward(5.0f);
    }
    else if (action == Action::MOVE_LEFT) {
        // turn left
        //owner->move_left(5.0f);
        owner->yaw_left(4.0f);
    }
    else if (action == Action::MOVE_RIGHT) {
        // turn right
        //owner->move_right(5.0f);
        owner->yaw_right(4.0f);
    }
    else if (action == Action::MOVE_UP) {
        // move up
        owner->move_up(5.0f);
    }
    else if (action == Action::MOVE_DOWN) {
        // move down
        owner->move_down(5.0f);
    }
    else if (action == Action::PITCH_UP) {
        // pitch up
        owner->pitch_up(3.0f);
    }
    else if (action == Action::PITCH_DOWN) {
        // pitch down
        owner->pitch_down(3.0f);
    }
    else if (action == Action::ROLL_LEFT) {
        // pitch up
        owner->roll_left(3.0f);
    }
    else if (action == Action::ROLL_RIGHT) {
        // pitch down
        owner->roll_right(3.0f);
    }
    else {
        return false;
    }
    return true;
}

bool vehicle_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, ivec3 rwc, Action action) {
    if (action == Action::MOUNTING) {
        game->player->unmount(game->world);
    }
    else {
        return false;
    }
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
}
