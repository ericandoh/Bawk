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
#include "includeglfw.h"
#include "entity.h"
#include "math.h"

class World;

class RenderablePlayer: public Entity {
    fmat4 mvp;
    fmat4 view, projection;
    GLuint cursor_vbo;
public:
    RenderablePlayer();
    fmat4* set_camera();
    void query_depth(World* world);
    // renders a box around the selection
    // also updates appropriate variables marking the selection
    void render();
};

#endif /* defined(__Bawk__playerrender__) */