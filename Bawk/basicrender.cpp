//
//  basicrender.cpp
//  Bawk
//
//  Some common geometries that might need to be rendered, such as cubes, rectangles, etc
//
//  Created by Eric Oh on 2/4/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "basicrender.h"
#include "worldrender.h"

GLbyte filled_cube_geometry[36][3] = {
    {-1,-1,-1},
    {-1,-1, 1},
    {-1, 1, 1},
    {1, 1,-1},
    {-1,-1,-1},
    {-1, 1,-1},
    {1,-1, 1},
    {-1,-1,-1},
    {1,-1,-1},
    {1, 1,-1},
    {1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1, 1, 1},
    {-1, 1,-1},
    {1,-1, 1},
    {-1,-1, 1},
    {-1,-1,-1},
    {-1, 1, 1},
    {-1,-1, 1},
    {1,-1, 1},
    {1, 1, 1},
    {1,-1,-1},
    {1, 1,-1},
    {1,-1,-1},
    {1, 1, 1},
    {1,-1, 1},
    {1, 1, 1},
    {1, 1,-1},
    {-1, 1,-1},
    {1, 1, 1},
    {-1, 1,-1},
    {-1, 1, 1},
    {1, 1, 1},
    {-1, 1, 1},
    {1,-1, 1}
};

GLbyte wire_cube_geometry[24][3] = {
    {-1,-1,-1},
    { 1,-1,-1},
    { 1,-1,-1},
    { 1,-1, 1},
    { 1,-1, 1},
    {-1,-1, 1},
    {-1,-1, 1},
    {-1,-1,-1},
    
    {-1,-1,-1},
    {-1, 1,-1},
    { 1,-1,-1},
    { 1, 1,-1},
    { 1,-1, 1},
    { 1, 1, 1},
    {-1,-1, 1},
    {-1, 1, 1},
    
    {-1, 1,-1},
    { 1, 1,-1},
    { 1, 1,-1},
    { 1, 1, 1},
    { 1, 1, 1},
    {-1, 1, 1},
    {-1, 1, 1},
    {-1, 1,-1}
};

void draw_filled_cube_geometry() {
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof filled_cube_geometry, filled_cube_geometry, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_BYTE);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void draw_wire_cube_geometry() {
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof wire_cube_geometry, wire_cube_geometry, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_BYTE);
    
    glDrawArrays(GL_LINES, 0, 24);
}

void bind_cube_transform(fmat4* transform, bounding_box* box) {
    fvec3 world_pos = box->get_world_pos();
    fmat4 view = glm::translate(fmat4(1), world_pos);
    // scale box to dimensions
    fvec3 dimensions = box->get_dimensions();
    // add offset so our box "floats" above the actual blocks
    view = glm::scale(view, fvec3(dimensions.x / 2.0f,
                                  dimensions.y / 2.0f,
                                  dimensions.z / 2.0f));
    view = *transform * view;
    OGLAttr::current_shader->set_transform_matrix(&view);
}

void draw_colored_cube(fmat4* transform, bounding_box* box, fvec3 color, bool transparent) {
    bind_cube_transform(transform, box);
    GLfloat box_texture[36][3];
    for (int i = 0; i < 36; i++) {
        box_texture[i][0] = color.x;
        box_texture[i][1] = color.y;
        box_texture[i][2] = color.z;
    }
    if (transparent) {
        OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR_TRANSPARENT);
    }
    else {
        OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    draw_filled_cube_geometry();
}

void draw_colored_wire_cube(fmat4* transform, bounding_box* box, fvec3 color, bool transparent) {
    bind_cube_transform(transform, box);
    GLfloat box_texture[24][3];
    for (int i = 0; i < 24; i++) {
        box_texture[i][0] = color.x;
        box_texture[i][1] = color.y;
        box_texture[i][2] = color.z;
    }
    if (transparent) {
        OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR_TRANSPARENT);
    }
    else {
        OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof box_texture, box_texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    draw_wire_cube_geometry();
}