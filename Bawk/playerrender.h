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
#include <GLFW/glfw3.h>
#include "basic_types.h"
#include "math.h"

class RenderablePlayer {
    fmat4 mvp;
    fmat4 view, projection;
    GLuint cursor_vbo;
protected:
    fvec3 pos;
    fvec3 forward;
    //fvec3 right;
    fvec3 up;
    fvec3 dir;
public:
    RenderablePlayer();
    void move_forward();
    void move_backward();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    fvec3* get_pos();
    fmat4* set_camera();
    void query_depth();
    // renders a box around the selection
    // also updates appropriate variables marking the selection
    void render();
};

#endif /* defined(__Bawk__playerrender__) */