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
//  -
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

class Player {
    Position pos;
    Direction dir;
    
    PlayerRender playerrender;
public:
    Player();
    void set_camera();
};

#endif /* defined(__Bawk__player__) */