//
//  playerrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "display.h"    // for get_window_size
#include "playerrender.h"

RenderablePlayer::RenderablePlayer() {
    // initialize things
    pos = fvec3(0.0f, 0.0f, 0.0f);
    dir = fvec3(1.0f, 0.0f, 0.0f);
    forward = dir;
    up = fvec3(0.0f, 1.0f, 0.0f);
}

// movement methods. Move these to class Entity
void RenderablePlayer::move_forward() {
    // override this if flying/on land
    pos += forward;
}
void RenderablePlayer::move_backward() {
    pos -= forward;
}
void RenderablePlayer::move_left() {
    pos.x += forward.z;
    pos.z -= forward.x;
}
void RenderablePlayer::move_right() {
    pos.x -= forward.z;
    pos.z += forward.x;
}
void RenderablePlayer::move_up() {
    pos.y += 1.0f;
}
void RenderablePlayer::move_down() {
    pos.y -= 1.0f;
}

fmat4* RenderablePlayer::set_camera() {
    int width, height;
    get_window_size(&width, &height);
    
    fmat4 view = glm::lookAt(pos, pos + dir, up);
    fmat4 projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    mvp = projection * view;
    return &mvp;
}