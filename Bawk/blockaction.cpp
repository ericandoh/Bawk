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
bool default_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int button) {
    return true;
}

bool engine_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int button) {
    return false;
}

bool vehicle_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int button) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!owner) {
            // remove this block from the world
            ivec3 irwc = get_floor_from_fvec3(rwc);
            game->world->kill_block(&irwc);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (owner && owner->entity_class == 4) {
            // there must be an owner!!!!!!!!!!!
            ivec3 irwc = get_floor_from_fvec3(rwc);
            game->player->set_mount((SuperObject*)owner, fvec3(irwc.x + 0.5f, irwc.y + 1.0f, irwc.z + 0.5f));
        }
    }
    else {
        return false;
    }
    return true;
}


// --- ALL KEYBOARD ACTIONS ---

// this method is never called, but serves as a template
bool default_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action action) {
    return true;
}

// a universal 6 directional engine
// drawbacks is it is very weak
bool engine_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action action) {
    if (action == Action::MOVE_UP) {
        // move up
        printf("KEY 0 PRSSED\n");
        owner->move_up(5.0f);
    }
    else if (action == Action::MOVE_DOWN) {
        // move down
        printf("KEY 1 PRSSED\n");
        owner->move_down(5.0f);
    }
    else if (action == Action::MOVE_LEFT) {
        // turn left
        //owner->move_left(5.0f);
        owner->turn_left(5.0f);
    }
    else if (action == Action::MOVE_RIGHT) {
        // turn right
        //owner->move_right(5.0f);
        owner->turn_right(5.0f);
    }
    else if (action == Action::MOVE_FORWARD) {
        // move forward
        owner->move_forward(5.0f);
    }
    else if (action == Action::MOVE_BACKWARD) {
        // move backwards
        owner->move_backward(5.0f);
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
        return engine_block_keyboard_callback;
    }
    else if (name.compare("vehicle") == 0) {
        // default keymaps for action 0, 1
        default_keymap.push_back(Action::MOUNTING);
        return vehicle_block_keyboard_callback;
    }
    return 0;
}
