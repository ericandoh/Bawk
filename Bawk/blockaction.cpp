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

// --- ALL MOUSE ACTIONS ---

// this method is never called, but serves as a template
bool default_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action key) {
    return true;
}

bool engine_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action key) {
    return false;
}

bool vehicle_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action key) {
    if (key == Action::CLICK_DESTROY) {
        return false;
    }
    else if (key == CLICK_CREATE) {
        if (owner && owner->entity_class == 3) {
            // there must be an owner!!!!!!!!!!!
            ivec3 irwc = get_floor_from_fvec3(rwc);
            game->player->set_mount((SuperObject*)owner, fvec3(irwc.x + 0.5f, irwc.y + 1.0f, irwc.z + 0.5f));
            return true;
        }
    }
    return false;
}


// --- ALL KEYBOARD ACTIONS ---

// this method is never called, but serves as a template
bool default_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action action) {
    return true;
}

// a universal 6 directional engine
// drawbacks is it is very weak
bool engine_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action action) {
    if (action == Action::MOVE_FORWARD) {
        // move forward
        owner->move_forward(50.0f);
    }
    else if (action == Action::MOVE_BACKWARD) {
        // move backwards
        owner->move_backward(5.0f);
    }
    else if (action == Action::MOVE_LEFT) {
        // turn left
        //owner->move_left(5.0f);
        owner->yaw_left(2.0f);
    }
    else if (action == Action::MOVE_RIGHT) {
        // turn right
        //owner->move_right(5.0f);
        owner->yaw_right(2.0f);
    }
    else if (action == Action::MOVE_UP) {
        // move up
        owner->move_up(2.0f);
    }
    else if (action == Action::MOVE_DOWN) {
        // move down
        owner->move_down(2.0f);
    }
    else if (action == Action::PITCH_UP) {
        // pitch up
        owner->pitch_up(2.0f);
    }
    else if (action == Action::PITCH_DOWN) {
        // pitch down
        owner->pitch_down(2.0f);
    }
    else if (action == Action::ROLL_LEFT) {
        // pitch up
        owner->roll_left(2.0f);
    }
    else if (action == Action::ROLL_RIGHT) {
        // pitch down
        owner->roll_right(2.0f);
    }
    else {
        return false;
    }
    return true;
}

bool vehicle_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action action) {
    if (action == Action::MOUNTING) {
        game->player->unmount(game->world);
    }
    else {
        return false;
    }
    return true;
}

// --- MOUSE/KEYBOARD ACTION FETCH METHODS

block_mouse_callback_func get_block_mouse_callback_for(std::string name) {
    if (name.compare("engine") == 0) {
        return engine_block_mouse_callback;
    }
    if (name.compare("vehicle") == 0) {
        return vehicle_block_mouse_callback;
    }
    return 0;
}

block_keyboard_callback_func get_block_keyboard_callback_for(std::string name, std::vector<Action> &default_keymap) {
    if (name.compare("engine") == 0) {
        // default keymaps for action 0, 1
        default_keymap.push_back(Action::MOVE_UP);
        default_keymap.push_back(Action::MOVE_DOWN);
        default_keymap.push_back(Action::MOVE_LEFT);
        default_keymap.push_back(Action::MOVE_RIGHT);
        default_keymap.push_back(Action::MOVE_FORWARD);
        default_keymap.push_back(Action::MOVE_BACKWARD);
        default_keymap.push_back(Action::PITCH_UP);
        default_keymap.push_back(Action::PITCH_DOWN);
        default_keymap.push_back(Action::ROLL_LEFT);
        default_keymap.push_back(Action::ROLL_RIGHT);
        return engine_block_keyboard_callback;
    }
    else if (name.compare("vehicle") == 0) {
        // default keymaps for action 0, 1
        default_keymap.push_back(Action::MOUNTING);
        return vehicle_block_keyboard_callback;
    }
    return 0;
}
