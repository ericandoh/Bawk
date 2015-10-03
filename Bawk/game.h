//
//  Main game class
//  - Holds the World, other entity information
//  - Multiplexes all functions together
//  - Holds a reference to the current Player playing this game
//
//  Used by:
//  - display.cpp (displays this)
//  - program.cpp (sets this to be displayed)
//
//  Uses:
//  - world.h, player.h
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
#include "displayable.h"

class Game : public Displayable {
    World* world;
    Player* player;
    fvec3 last_player_pos;
    void check_need_update();
public:
    int init() override;
    void render() override;
    void key_callback(int key, int scancode, int action, int mods) override;
    void mouse_move_callback(double xdiff, double ydiff) override;
    ~Game();
};

#endif /* defined(__Bawk__game__) */