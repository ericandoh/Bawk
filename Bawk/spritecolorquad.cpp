//
//  spritecolorquad.cpp
//  Bawk
//
//  Created by Eric Oh on 12/20/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spritecolorquad.h"
#include "worldrender.h"
#include "player.h"

SpriteColorQuad::SpriteColorQuad() {
    
}

void SpriteColorQuad::render(fmat4* transform, SpriteRender* render)  {
    render->pos = render->player->pos;
    render->recalculate_transform();
    
    // drawing direct colors
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
    // bind a rectangle pointing in direction of player, at pos
    fmat4 view;
    render->get_mvp(&view);
    view = *transform * view;
    OGLAttr::current_shader->set_transform_matrix(&view);
    
    int num_vertices = 6;
    
    fvec3 rectangle_vertex_data[6];
    rectangle_vertex_data[0] = corners[0];
    rectangle_vertex_data[1] = corners[1];
    rectangle_vertex_data[2] = corners[2];
    rectangle_vertex_data[3] = corners[2];
    rectangle_vertex_data[4] = corners[1];
    rectangle_vertex_data[5] = corners[3];
    
    fvec3 texture_binds[6];
    texture_binds[0] = colors[0];
    texture_binds[1] = colors[1];
    texture_binds[2] = colors[2];
    texture_binds[3] = colors[2];
    texture_binds[4] = colors[1];
    texture_binds[5] = colors[3];
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof rectangle_vertex_data, &(rectangle_vertex_data[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture_binds, &(texture_binds[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

SpriteRenderable* SpriteColorQuad::copy()  {
    SpriteColorQuad* copy = new SpriteColorQuad();
    for (int i = 0; i < 4; i++) {
        copy->colors[i] = colors[i];
        copy->corners[i] = corners[i];
    }
    return copy;
}
