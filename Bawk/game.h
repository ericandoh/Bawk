//
//  Main game class
//  - Holds the World, other entity information
//  - Multiplexes all functions together
//  - Main logic loop can be found here
//  - Holds a reference to the current Player playing this game
//
//  Used by:
//  - main.cpp (game entry point)
//
//  Uses:
//  -
//
//  Notes
//  - Move this into a Screen subclass then run another global instance on top of this
//  - That will allow us to have loading screen/menu/etc
//  - If we do multiplayer later, this will have to be split into a client-side and server-side
//    handling (and we would override world with a dummy reference that would query)
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game__
#define __Bawk__game__

#include <stdio.h>
#include <GLFW/glfw3.h>
#include "world.h"
#include "player.h"

class Game {
    World world;
    Player player;
public:
    void init();
    void render();
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif /* defined(__Bawk__game__) */