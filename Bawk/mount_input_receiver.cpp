//
//  mount_input_receiver.cpp
//  Bawk
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "mount_input_receiver.h"
#include "client_accessor.h"
#include "superobject.h"

bool MountInputReceiver::has_mount() {
    return get_player()->get_mount();
}

bool MountInputReceiver::key_callback(Action do_this, int ms) {
    if (has_mount()) {
        return get_player()->get_mount()->block_keyboard_callback(get_player(), do_this, get_player()->get_mount(), ms);
    }
    return false;
}

bool MountInputReceiver::mouse_move_callback(double xdiff, double ydiff) {
    // TOFU implement code to move the mount around the world
    return false;
}

//bool mouse_clicked_callback(Action do_this) {}

bool MountInputReceiver::mouse_clicking_callback(Action do_this, int ms) {
    if (has_mount()) {
        int ms = 3;
        return get_player()->get_mount()->block_keyboard_callback(get_player(), do_this, get_player()->get_mount(), ms);
    }
    return false;
}

//bool scroll_callback(double xdiff, double ydiff) {}