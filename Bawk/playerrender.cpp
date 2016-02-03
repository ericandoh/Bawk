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
    angle.free_y = false;
}

/*void RenderablePlayer::recalculate_dir() {
    if(angle.y < -STRAIGHT_ANGLE)
        angle.y = -STRAIGHT_ANGLE;
    if(angle.y > STRAIGHT_ANGLE)
        angle.y = STRAIGHT_ANGLE;
    Entity::recalculate_dir();
}*/

void RenderablePlayer::set_camera() {
    int width, height;
    get_window_size(&width, &height);
    fvec3 offpos = get_center_pos();
    // TODO angle.dir is not relative...
    Rotation lookdir;
    get_direction(&lookdir);
    view = glm::lookAt(offpos, offpos + lookdir.dir, lookdir.up);
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
    fvec3 offpos = get_center_pos();
    Rotation lookdir;
    get_direction(&lookdir);
    set_look_at(offpos, lookdir.dir, world);
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