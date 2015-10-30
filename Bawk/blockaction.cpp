//
//  blockaction.cpp
//  Bawk
//
//  Created by Eric Oh on 10/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <GLFW/glfw3.h>
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
            game->player->set_mount((SuperObject*)owner, fvec3(rwc.x, rwc.y + 0.5f, rwc.z));
        }
    }
    else {
        return false;
    }
    return true;
}


// --- ALL KEYBOARD ACTIONS ---

// this method is never called, but serves as a template
bool default_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int action) {
    return true;
}

// a universal 6 directional engine
// drawbacks is it is very weak
bool engine_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int action) {
    if (action == 0) {
        // move up
        printf("KEY 0 PRSSED\n");
        owner->move_up(5.0f);
    }
    else if (action == 1) {
        // move down
        printf("KEY 1 PRSSED\n");
        owner->move_down(5.0f);
    }
    else if (action == 2) {
        // turn left
        //owner->move_left(5.0f);
        owner->turn_left(5.0f);
    }
    else if (action == 3) {
        // turn right
        //owner->move_right(5.0f);
        owner->turn_right(5.0f);
    }
    else if (action == 4) {
        // move forward
        owner->move_forward(5.0f);
    }
    else if (action == 5) {
        // move backwards
        owner->move_backward(5.0f);
    }
    else {
        return false;
    }
    return true;
}

bool vehicle_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int action) {
    if (action == 0) {
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

block_keyboard_callback_func get_block_keyboard_callback_for(std::string name, std::vector<int> &default_keymap) {
    if (name.compare("engine") == 0) {
        // default keymaps for action 0, 1
        default_keymap.push_back(GLFW_KEY_Q);
        default_keymap.push_back(GLFW_KEY_Z);
        default_keymap.push_back(GLFW_KEY_A);
        default_keymap.push_back(GLFW_KEY_D);
        default_keymap.push_back(GLFW_KEY_W);
        default_keymap.push_back(GLFW_KEY_S);
        default_keymap.push_back(GLFW_KEY_H);
        return engine_block_keyboard_callback;
    }
    else if (name.compare("vehicle") == 0) {
        // default keymaps for action 0, 1
        default_keymap.push_back(GLFW_KEY_Y);
        return vehicle_block_keyboard_callback;
    }
    return 0;
}
