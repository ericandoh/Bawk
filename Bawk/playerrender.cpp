//
//  playerrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>
#include "display.h"    // for get_window_size
#include "worldrender.h"
#include "blocktracer.h"
#include "playerrender.h"
#include "block.h"

RenderablePlayer::RenderablePlayer() {
    // initialize things
    pos = fvec3(0.0f, 8.0f, 0.0f);
    dir = fvec3(1.0f, 0.0f, 0.0f);
    up = fvec3(0.0f, 1.0f, 0.0f);
    
    //lower_bound = fvec3(-1.0f, -1.0f, -1.0f);
    //upper_bound = fvec3(0.0f, 0.0f, 0.0f);
    lower_bound = fvec3(-0.4f, -0.9f, -0.4f);
    upper_bound = fvec3(0.4f, 0.9f, 0.4f);
    
    glGenBuffers(1, &cursor_vbo);
    // TODO do we ever free this...
}

fmat4* RenderablePlayer::set_camera() {
    int width, height;
    get_window_size(&width, &height);
    
    view = glm::lookAt(pos, pos + dir, up);
    projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    mvp = projection * view;
    return &mvp;
}

void RenderablePlayer::query_depth(World* world) {
    int width, height;
    float depth;
    get_window_size(&width, &height);
    
    view = glm::lookAt(pos, pos + dir, up);
    projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    mvp = projection * view;
    
    
    // gets the depth at the place in the screen we're pointing at
    glReadPixels(width / 2, height / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    
    glm::vec4 viewport = glm::vec4(0, 0, width, height);
    glm::vec3 wincoord = glm::vec3(width / 2, height / 2, depth);
    // view, projection matrices should be set from previous set_camera() call
    glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);
    
    
    // call to save mx, my, mz, and face here to a global var
    set_look_at(pos, dir, depth, objcoord, world);
}

void RenderablePlayer::render() {
    int width, height;
    get_window_size(&width, &height);
    
    /* Draw a cross in the center of the screen */
    float cross[4][3] = {
        {-0.05f, 0, 0},
        {+0.05f, 0, 0},
        {0, -0.05f * width/height, 0},
        {0, +0.05f * width/height, 0},
    };
    
    glm::mat4 one(1);
    set_transform_matrix(one);
    glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, 4);
}