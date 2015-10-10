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
#include "playerrender.h"

RenderablePlayer::RenderablePlayer() {
    // initialize things
    pos = fvec3(0.0f, 8.0f, 0.0f);
    dir = fvec3(1.0f, 0.0f, 0.0f);
    forward = dir;
    up = fvec3(0.0f, 1.0f, 0.0f);
    glGenBuffers(1, &cursor_vbo);
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

fvec3* RenderablePlayer::get_pos() {
    return &pos;
}

fmat4* RenderablePlayer::set_camera() {
    int width, height;
    get_window_size(&width, &height);
    
    view = glm::lookAt(pos, pos + dir, up);
    projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    mvp = projection * view;
    return &mvp;
}

static float dti(float val) {
    return fabsf(val - roundf(val));
}

void RenderablePlayer::query_depth() {
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
    
    int mx = floorf(objcoord.x);
    int my = floorf(objcoord.y);
    int mz = floorf(objcoord.z);
    
    float distance = sqrtf((mx-pos.x)*(mx-pos.x) + (my-pos.y)*(my-pos.y) + (mz-pos.z)*(mz-pos.z));
    
    //int nx = roundf(objcoord.x);
    //int ny = roundf(objcoord.y);
    //int nz = roundf(objcoord.z);
    
    int face;
    
    if(dti(objcoord.x) < dti(objcoord.y)) {
        if(dti(objcoord.x) < dti(objcoord.z)) {
            // face is perpendicular to x-axis (most likely)
            if(dir.x > 0) {
                //mx = nx;
                face = 3;
            } else {
                //mx = nx;
                face = 0;
            }
        } else {
            // face is perpendicular to z-axis (most likely)
            if(dir.z > 0) {
                //mz = nz;
                face = 5;
            } else {
                //mz = nz;
                face = 2;
            }
        }
    } else {
        if(dti(objcoord.y) < dti(objcoord.z)) {
            // face is perpendicular to y-axis (most likely)
            if(dir.y > 0) {
                //my = ny;
                face = 4;
            } else {
                //my = ny;
                face = 1;
            }
        } else {
            // face is perpendicular to z-axis (most likely)
            if(dir.z > 0) {
                //mz = nz;
                face = 5;
            } else {
                //mz = nz;
                face = 2;
            }
        }
    }
    // call to save mx, my, mz, and face here to a global var
    set_look_at(distance, mx, my, mz, face);
}

void RenderablePlayer::render() {
    int width, height;
    get_window_size(&width, &height);
    /*
    float bx = mx;
    float by = my;
    float bz = mz;
    
    // Render a box around the block we are pointing at
    float box[24][3] = {
        {bx + 0, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 0, by + 1, bz + 0},
        {bx + 1, by + 1, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 1, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 1},
        
        {bx + 0, by + 0, bz + 0},
        {bx + 0, by + 1, bz + 0},
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 1, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 0, bz + 1},
        {bx + 1, by + 1, bz + 1},
        
        {bx + 0, by + 0, bz + 0},
        {bx + 0, by + 0, bz + 1},
        {bx + 1, by + 0, bz + 0},
        {bx + 1, by + 0, bz + 1},
        {bx + 0, by + 1, bz + 0},
        {bx + 0, by + 1, bz + 1},
        {bx + 1, by + 1, bz + 0},
        {bx + 1, by + 1, bz + 1},
    };
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_CULL_FACE);
    
    set_transform_matrix(mvp);
    
    glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, 24);*/
    
    /* Draw a cross in the center of the screen */
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_CULL_FACE);
    
    float cross[4][3] = {
        {-0.05f, 0, 0},
        {+0.05f, 0, 0},
        {0, -0.05f * width/height, 0},
        {0, +0.05f * width/height, 0},
    };
    
    glDisable(GL_DEPTH_TEST);
    glm::mat4 one(1);
    set_transform_matrix(one);
    glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);
    glVertexPointer(4, GL_FLOAT, 0, cross);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, 4);
}