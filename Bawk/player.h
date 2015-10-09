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
#include "math.h"
#include "playerrender.h"
#include "basic_types.h"

class Player : public RenderablePlayer {
    fvec2 angle;
public:
    Player();
    void update_direction(double xdiff, double ydiff);
    ivec3 get_rounded_left();
    ivec3 get_rounded_forward();
    void debug();
};

#endif /* defined(__Bawk__player__) */