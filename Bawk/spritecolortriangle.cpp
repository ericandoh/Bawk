//
//  spritecolortriangle.cpp
//  Bawk
//
//  Created by Eric Oh on 12/20/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spritecolortriangle.h"

#include "worldrender.h"
#include "player.h"

SpriteColorTriangle::SpriteColorTriangle() {
    
}

void SpriteColorTriangle::render(fmat4* transform, SpriteRender* render)  {
    render->pos = render->player->pos;
    render->recalculate_transform();
    
    // drawing direct colors
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
    // bind a rectangle pointing in direction of player, at pos
    fmat4 view;
    render->get_mvp(&view);
    view = *transform * view;
    OGLAttr::current_shader->set_transform_matrix(&view);
    
    int num_vertices = 3;
    
    fvec3 rectangle_vertex_data[3];
    rectangle_vertex_data[0] = corners[0];
    rectangle_vertex_data[1] = corners[1];
    rectangle_vertex_data[2] = corners[2];
    
    fvec3 texture_binds[3];
    texture_binds[0] = colors[0];
    texture_binds[1] = colors[1];
    texture_binds[2] = colors[2];
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof rectangle_vertex_data, &(rectangle_vertex_data[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture_binds, &(texture_binds[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

SpriteRenderable* SpriteColorTriangle::copy()  {
    SpriteColorTriangle* copy = new SpriteColorTriangle();
    for (int i = 0; i < 3; i++) {
        copy->colors[i] = colors[i];
        copy->corners[i] = corners[i];
    }
    return copy;
}