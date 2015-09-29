//
//  game.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "game.h"
#include "world.h"
#include "player.h"

World world;
Player player;

namespace game {
    // initializes all needed game variables. This should be called before render()
    void init() {
        world = World();
        // later separate loading the player and the world - this might get complicated!
        player = Player();
    }
    
    // Called in the main render loop. Pass the rendering to the appropriate entries
    void render() {
        player.set_camera();
        world.render();
    }
}