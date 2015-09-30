//
//  game.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "game.h"
#include <map>

enum Action {
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_FORWARD, MOVE_BACKWARD
};

std::map<int, Action> key_to_action;

// initializes all needed game variables. This should be called before render()
void Game::init() {
    world = World();
    // later separate loading the player and the world - this might get complicated!
    player = Player();
    
    key_to_action[GLFW_KEY_SPACE] = MOVE_UP;
    key_to_action[GLFW_KEY_LEFT_SHIFT] = MOVE_DOWN;
    key_to_action[GLFW_KEY_A] = MOVE_LEFT;
    key_to_action[GLFW_KEY_D] = MOVE_RIGHT;
    key_to_action[GLFW_KEY_W] = MOVE_FORWARD;
    key_to_action[GLFW_KEY_S] = MOVE_BACKWARD;
    
}
    
// Called in the main render loop. Pass the rendering to the appropriate entries
void Game::render() {
    player.set_camera();
    world.render();
}

// Calls an action depending on key pressed
// Not all key presses may correspond to player so this method belongs here
void Game::key_callback(int key, int scancode, int action, int mods) {
    // http://www.glfw.org/docs/latest/input.html#input_keyboard
    // http://www.glfw.org/docs/latest/quick.html
    
    if (action == GLFW_PRESS) {
        Action todo = key_to_action[key];
        
        switch (todo) {
            case MOVE_UP:
                player.move_up();
                break;
            case MOVE_DOWN:
                player.move_down();
                break;
            case MOVE_LEFT:
                player.move_left();
                break;
            case MOVE_RIGHT:
                player.move_right();
                break;
            case MOVE_FORWARD:
                player.move_forward();
                break;
            case MOVE_BACKWARD:
                player.move_backward();
                break;
            default:
                // do nothing
                ;
        }
        player.debug();
    }
}

void Game::mouse_move_callback(double xdiff, double ydiff) {
    player.update_direction(xdiff, ydiff);
}

