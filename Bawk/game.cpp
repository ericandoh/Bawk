//
//  game.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "game.h"



// initializes all needed game variables. This should be called before render()
void Game::init() {
    world = World();
    // later separate loading the player and the world - this might get complicated!
    player = Player();
}
    
// Called in the main render loop. Pass the rendering to the appropriate entries
void Game::render() {
    player.set_camera();
    world.render();
}


void Game::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
}

