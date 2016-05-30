//
//  inputmapper.cpp
//  Bawk
//
//  Created by Eric Oh on 5/22/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "inputmapper.h"
#include <map>
#include "importsdl.h"

namespace UIValues {
    int toggleable_keys[] = {SDLK_SPACE,
        SDLK_c, SDLK_a, SDLK_d,
        SDLK_w, SDLK_s, SDLK_TAB, SDLK_LSHIFT,
        SDLK_q, SDLK_e};
    
    std::map<int, Action> key_to_action;
    std::map<int, Action> mouse_to_action;
    std::map<int, bool> key_toggled;
    std::map<int, bool> mouse_toggled;
}

using namespace UIValues;

void initInputMapping() {
    // set key mappings
    // movement key mappings
    key_to_action[SDLK_SPACE] = MOVE_UP;
    key_to_action[SDLK_c] = MOVE_DOWN;
    key_to_action[SDLK_a] = MOVE_LEFT;
    key_to_action[SDLK_d] = MOVE_RIGHT;
    key_to_action[SDLK_w] = MOVE_FORWARD;
    key_to_action[SDLK_s] = MOVE_BACKWARD;
    
    key_to_action[SDLK_TAB] = PITCH_UP;
    key_to_action[SDLK_LSHIFT] = PITCH_DOWN;
    key_to_action[SDLK_q] = ROLL_LEFT;
    key_to_action[SDLK_e] = ROLL_RIGHT;
    
    key_to_action[SDLK_RETURN] = CONFIRM;
    key_to_action[SDLK_ESCAPE] = CANCEL;
    
    key_to_action[SDLK_PERIOD] = MOVE_BLOCK_UP;
    key_to_action[SDLK_SLASH] = MOVE_BLOCK_DOWN;
    key_to_action[SDLK_LEFT] = MOVE_BLOCK_LEFT;
    key_to_action[SDLK_RIGHT] = MOVE_BLOCK_RIGHT;
    key_to_action[SDLK_UP] = MOVE_BLOCK_FORWARD;
    key_to_action[SDLK_DOWN] = MOVE_BLOCK_BACKWARD;
    key_to_action[SDLK_SEMICOLON] = MOVE_BLOCK_ROTATE;
    
    key_to_action[SDLK_i] = OPEN_INV;
    key_to_action[SDLK_b] = SAVE_TEMPLATE;
    key_to_action[SDLK_v] = TOGGLE_VIEWPOINT;
    
    key_to_action[SDLK_m] = DEBUG_ACTION;
    
    key_to_action[SDLK_y] = MOUNTING;
    
    key_to_action[SDLK_DELETE] = DELETE;
    
    // is there a less stupid way to do this lol
    key_to_action[SDLK_1] = KEY0;
    key_to_action[SDLK_2] = KEY1;
    key_to_action[SDLK_3] = KEY2;
    key_to_action[SDLK_4] = KEY3;
    key_to_action[SDLK_5] = KEY4;
    key_to_action[SDLK_6] = KEY5;
    key_to_action[SDLK_7] = KEY6;
    key_to_action[SDLK_8] = KEY7;
    key_to_action[SDLK_9] = KEY8;
    key_to_action[SDLK_0] = KEY9;
    
    
    mouse_to_action[SDL_BUTTON_LEFT] = CLICK_MAIN;
    mouse_to_action[SDL_BUTTON_RIGHT] = CLICK_SECONDARY;
    
    for (unsigned int i = 0; i < sizeof(toggleable_keys); i++) {
        key_toggled[toggleable_keys[i]] = false;
    }
    for (auto &i: mouse_to_action) {
        mouse_toggled[i.first] = false;
    }
}

Action getKeyActionFrom(int key) {
    return key_to_action[key];
}

Action getMouseActionFrom(int key) {
    return mouse_to_action[key];
}

bool isKeyToggleable(int key) {
    return key_toggled.count(key);
}

bool isMouseToggleable(int key) {
    return mouse_toggled.count(key);
}

void toggleKey(int key, bool pressed) {
    key_toggled[key] = pressed;
}

void toggleMouse(int key, bool pressed) {
    mouse_toggled[key] = pressed;
}

void stepInput(Displayable* displayable, int ms) {
    for (auto& key : key_toggled) {
        if (key.second) {
            Action do_this = key_to_action[key.first];
            displayable->key_callback(do_this, ms);
        }
    }
    
    for (auto &key: mouse_toggled) {
        if (key.second) {
            Action do_this = mouse_to_action[key.first];
            displayable->mouse_clicking_callback(do_this, ms);
        }
    }

}
