//
//  modeldata.cpp
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "modeldata.h"
#include "worldrender.h"

void ModelData::render() {
    // draw the model elements
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::UV);
    int num_triangles = (int)model_vertices.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, model_vertices.size() * sizeof(fvec3), &(model_vertices[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, model_uvs.size() * sizeof(fvec3), &(model_uvs[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    glDrawArrays(GL_TRIANGLES, 0, num_triangles);
}