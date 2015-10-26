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

// --- ALL MOUSE ACTIONS ---

// this method is never called, but serves as a template
bool default_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int button) {
    return true;
}

bool engine_block_mouse_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int button) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        printf("engine was clicked! :D with left\n");
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        printf("engine was clicked with right\n");
    return true;
}


// --- ALL KEYBOARD ACTIONS ---

// this method is never called, but serves as a template
bool default_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int action) {
    return true;
}

bool engine_block_keyboard_callback(Game* game, Entity* owner, block_type* blk, fvec3 rwc, int action) {
    if (action == 0) {
        printf("KEY 0 PRSSED\n");
    }
    else if (action == 1) {
        printf("KEY 1 PRSSED\n");
    }
    return true;
}


// --- MOUSE/KEYBOARD ACTION FETCH METHODS

block_mouse_callback_func get_block_mouse_callback_for(std::string name) {
    if (name.compare("engine") == 0) {
        return engine_block_mouse_callback;
    }
    return 0;
}

block_keyboard_callback_func get_block_keyboard_callback_for(std::string name, std::vector<int> default_keymap) {
    if (name.compare("engine") == 0) {
        // default keymaps for action 0, 1
        default_keymap.push_back(GLFW_KEY_W);
        default_keymap.push_back(GLFW_KEY_A);
        return engine_block_keyboard_callback;
    }
    return 0;
}
