
//
//  input_multiplexer.cpp
//  Bawk
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//


#include "input_multiplexer.h"

void InputMultiplexer::add_receiver(Displayable* r) {
    // first, see if this receiver is already in our list
    for (int i = 0; i < receivers.size(); i++) {
        Displayable* receiver = receivers[i];
        if (receiver == r) {
            // move this to the end
            receivers.erase(receivers.begin() + i);
            receivers.push_back(r);
            return;
        }
    }
    // receiver was not found, so just add it
    receivers.push_back(r);
}

void InputMultiplexer::remove_receiver(Displayable* r) {
    for (int i = 0; i < receivers.size(); i++) {
        Displayable* receiver = receivers[i];
        if (receiver == r) {
            // move this to the end
            receivers.erase(receivers.begin() + i);
            return;
        }
    }
}

void InputMultiplexer::pop_receiver() {
    receivers.pop_back();
}


// --- Displayable ---
void InputMultiplexer::init() {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        receiver->init();
    }
}

void InputMultiplexer::cleanup() {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        receiver->cleanup();
    }
}

void InputMultiplexer::render() {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        receiver->render();
    }
}

void InputMultiplexer::render_lights() {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        receiver->render_lights();
    }
}

void InputMultiplexer::render_shadows() {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        receiver->render_shadows();
    }
}

void InputMultiplexer::frame(int ms) {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        receiver->frame(ms);
    }
}

bool InputMultiplexer::key_callback(Action do_this, int ms) {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        if (receiver->key_callback(do_this, ms)) {
            return true;
        }
    }
    return false;
}

bool InputMultiplexer::mouse_move_callback(double xdiff, double ydiff) {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        if (receiver->mouse_move_callback(xdiff, ydiff)) {
            return true;
        }
    }
    return false;
}

bool InputMultiplexer::mouse_clicked_callback(Action do_this) {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        if (receiver->mouse_clicked_callback(do_this)) {
            return true;
        }
    }
    return false;
}

bool InputMultiplexer::mouse_clicking_callback(Action do_this, int ms) {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        if (receiver->mouse_clicking_callback(do_this, ms)) {
            return true;
        }
    }
    return false;
}

bool InputMultiplexer::scroll_callback(double xdiff, double ydiff) {
    for (int i = (int)receivers.size() - 1; i >= 0; i--) {
        Displayable* receiver = receivers[i];
        if (receiver->scroll_callback(xdiff, ydiff)) {
            return true;
        }
    }
    return false;
}