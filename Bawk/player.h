//
//  Class holding information about a player
//  - Holds location + orientation
//  -
//  -
//
//  Used by:
//  - world.cpp
//  - game.cpp
//
//  Uses:
//  - math.h
//  -
//  -
//
//  Notes
//  - Later separate into a Player object (for other players) and MyPlayer (for player playing the game)
//  -
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__player__
#define __Bawk__player__

#include <stdio.h>
#include "glm/glm.hpp"
#include "math.h"
#include "playerrender.h"

class Player {
    glm::vec3 pos;
    glm::vec3 forward;
    //glm::vec3 right;
    glm::vec3 up;
    glm::vec3 dir;
    glm::vec2 angle;
    
    PlayerRender playerrender;
public:
    Player();
    void set_camera();
    void move_forward();
    void move_backward();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void update_direction(double xdiff, double ydiff);
    void debug();
};

#endif /* defined(__Bawk__player__) */