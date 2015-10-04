//
//  game.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <map>
#include "game.h"
#include "program.h"

enum Action {
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_FORWARD, MOVE_BACKWARD
};

int toggleable_keys[] = {GLFW_KEY_SPACE,
                            GLFW_KEY_Z, GLFW_KEY_A, GLFW_KEY_D,
                            GLFW_KEY_W, GLFW_KEY_S};

std::map<int, Action> key_to_action;
std::map<int, bool> key_toggled;

// if the player moves more than 1 dimension away from previous location, update
float CHUNK_UPDATE_TRIGGER_DISTANCE = 16.0f;

// initializes all needed game variables. This should be called before render()
int Game::init() {
    world = new World("testworld");
    
    // load resources for the world
    if (world->load_resources()) {
        return -1;
    }
    
    // later separate loading the player and the world - this might get complicated!
    player = new Player();
    fvec3* player_pos = player->get_pos();
    last_player_pos = fvec3(player_pos->x, player_pos->y, player_pos->z);
    
    world->update_chunks(0, player_pos);
    
    // set key mappings
    key_to_action[GLFW_KEY_SPACE] = MOVE_UP;
    key_to_action[GLFW_KEY_Z] = MOVE_DOWN;
    key_to_action[GLFW_KEY_A] = MOVE_LEFT;
    key_to_action[GLFW_KEY_D] = MOVE_RIGHT;
    key_to_action[GLFW_KEY_W] = MOVE_FORWARD;
    key_to_action[GLFW_KEY_S] = MOVE_BACKWARD;
    
    for (unsigned int i = 0; i < sizeof(toggleable_keys); i++) {
        key_toggled[toggleable_keys[i]] = false;
    }
    
    return 0;
}
    
// Called in the main render loop. Pass the rendering to the appropriate entries
void Game::render() {
    fmat4* transform = player->set_camera();
    world->render(transform);
}

// runs one frame of the game
void Game::frame() {
    bool need_update = false;
    for (auto& key : key_toggled) {
        if (key.second) {
            bool this_key_need_update = true;
            Action do_this = key_to_action[key.first];
            switch (do_this) {
                case MOVE_UP:
                    player->move_up();
                    break;
                case MOVE_DOWN:
                    player->move_down();
                    break;
                case MOVE_LEFT:
                    player->move_left();
                    break;
                case MOVE_RIGHT:
                    player->move_right();
                    break;
                case MOVE_FORWARD:
                    player->move_forward();
                    break;
                case MOVE_BACKWARD:
                    player->move_backward();
                    break;
                default:
                    // do nothing
                    this_key_need_update = false;
                    ;
            }
            need_update = need_update | this_key_need_update;
        }
    }
    if (need_update)
        check_need_update();
}

float get_dst(fvec3* a, fvec3* b) {
    return sqrtf(powf(a->x - b->x, 2) + powf(a->y - b->y, 2) + powf(a->z - b->z, 2));
}

void Game::check_need_update() {
    fvec3* player_pos = player->get_pos();
    if (get_dst(&last_player_pos, player_pos) >= CHUNK_UPDATE_TRIGGER_DISTANCE) {
        world->update_chunks(&last_player_pos, player_pos);
        last_player_pos = fvec3(player_pos->x, player_pos->y, player_pos->z);
    }
}

// Calls an action depending on key pressed
// Not all key presses may correspond to player so this method belongs here
void Game::key_callback(int key, int scancode, int action, int mods) {
    // http://www.glfw.org/docs/latest/input.html#input_keyboard
    // http://www.glfw.org/docs/latest/quick.html
    
    if (key == GLFW_KEY_ESCAPE) {
        exit_game();
    }
    if (key_toggled.count(key)) {
        if (action == GLFW_PRESS) {
            key_toggled[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            key_toggled[key] = false;
        }
    }
    
    if (action == GLFW_REPEAT) {
        
    }
}

void Game::mouse_move_callback(double xdiff, double ydiff) {
    player->update_direction(xdiff, ydiff);
}

Game::~Game() {
    printf("Cleaning up game\n");
    // cleanup assets
    world->free_resources();
    delete player;
    delete world;
}

