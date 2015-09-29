//
//  Render for player
//  - Sets + Readies camera for rendering
//  -
//  -
//
//  Used by:
//  - game.cpp
//
//  Uses:
//  - OpenGL
//  -
//  -
//
//  Notes
//  - Does NOT render the player! Only sets the camera transformation
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__playerrender__
#define __Bawk__playerrender__

#include <stdio.h>
#include "math.h"

class PlayerRender {
public:
    void set_camera(Position* pos, Direction* dir);
};

#endif /* defined(__Bawk__playerrender__) */