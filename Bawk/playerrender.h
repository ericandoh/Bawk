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
#include "basic_types.h"
#include "math.h"

class PlayerRender {
    fmat4 view;
    fmat4 projection;
    fmat4 mvp;
public:
    PlayerRender();
    fmat4* set_camera(fvec3* pos, fvec3* forward, fvec3* up, fvec3* dir);
};

#endif /* defined(__Bawk__playerrender__) */