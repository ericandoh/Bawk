//
//  modelrender.cpp
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelrender.h"
#include "worldrender.h"

RenderableModel::RenderableModel() {
    name = "";
    resistance = 0;
    weight = 0;
    vehicle = 0;
    bounds = bounding_box();
    center_pos = fvec3(0,0,0);
}

void RenderableModel::render() {
    // draw the model elements
    set_block_draw_mode(0);
    int num_triangles = (int)model_vertices.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, model_vertices.size() * sizeof(fvec3), &(model_vertices[0]), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, model_uvs.size() * sizeof(fvec3), &(model_uvs[0]), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_texture_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, num_triangles);
}

void RenderableModel::refresh() {
    // TODO calculate the below
    //bounding_box bounds;
    //fvec3 center_pos;
}