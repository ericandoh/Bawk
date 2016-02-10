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
    // TODO investigate if we set this when we set angle...
    // TODO make angle constricted to 90/-90 for y (no turning backwards on head)
    viewpoint = true;
}

/*void RenderablePlayer::recalculate_dir() {
    if(angle.y < -STRAIGHT_ANGLE)
        angle.y = -STRAIGHT_ANGLE;
    if(angle.y > STRAIGHT_ANGLE)
        angle.y = STRAIGHT_ANGLE;
    Entity::recalculate_dir();
}*/

bounding_box RenderablePlayer::get_bounds_for_viewing() {
    return bounds;
}

void RenderablePlayer::set_camera(bool viewpoint) {
    int width, height;
    get_window_size(&width, &height);
    fvec3 viewpoint_pos = get_world_pos();
    this->viewpoint = viewpoint;
    Rotation lookdir = get_world_rotation();
    fvec3 lookdirection = lookdir.get_dir();
    if (!viewpoint) {
        // 3rd person viewpoint
        bounding_box range = get_bounds_for_viewing();
        fvec3 height_offset = lookdir.get_up() * (range.upper.y - range.lower.y) * 0.3f;
        fvec3 backward_offset = lookdirection * (range.upper.x - range.lower.x) * (-3.0f);
        viewpoint_pos = viewpoint_pos + height_offset + backward_offset;
    }
    fvec3 up = fvec3(0,1,0);
    if (lookdir.get_up().y < 0) {
        up = fvec3(0,-1,0);
    }
    view = glm::lookAt(viewpoint_pos, viewpoint_pos + lookdirection, up);
    projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    mvp = projection * view;
    set_camera_transform_matrix(&mvp);
}

void RenderablePlayer::query_depth(World* world) {
    /*int width, height;
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
    glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);*/
    
    
    // call to save mx, my, mz, and face here to a global var
    fvec3 offpos = get_world_pos();
    Rotation lookdir = get_world_rotation();
    set_look_at(offpos, lookdir.get_dir(), world);
}

fvec3 RenderablePlayer::get_viewpoint() {
    fvec3 viewpoint_pos = get_world_pos();
    if (!viewpoint) {
        // 3rd person viewpoint
        Rotation lookdir = get_world_rotation();
        fvec3 lookdirection = lookdir.get_dir();
        bounding_box range = get_bounds_for_viewing();
        fvec3 height_offset = lookdir.get_up() * (range.upper.y - range.lower.y) * 0.3f;
        fvec3 backward_offset = lookdirection * (range.upper.x - range.lower.x) * (-3.0f);
        viewpoint_pos = viewpoint_pos + height_offset + backward_offset;
    }
    return viewpoint_pos;
}

void RenderablePlayer::render() {
    int width, height;
    get_window_size(&width, &height);
    
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
    
    /* Draw a cross in the center of the screen */
    float cross[4][3] = {
        {-0.05f, 0, 0},
        {+0.05f, 0, 0},
        {0, -0.05f * width/height, 0},
        {0, +0.05f * width/height, 0},
    };
    
    float cross_colors[4][3] = {
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
    };
    
    set_unitary_transform_matrix();
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof cross_colors, cross_colors, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_LINES, 0, 4);
}