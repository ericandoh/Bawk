//
//  modelrender.cpp
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelrender.h"
#include "worldrender.h"
#include "blocktracer.h"

RenderableModel::RenderableModel() {
    name = "";
    resistance = 0;
    weight = 0;
    vehicle = 0;
    bounds = bounding_box();
    center_pos = fvec3(0,0,0);
    light = RenderableLight();
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
    bounds = bounding_box();
    for (auto &i: model_vertices) {
        bounds.expand(i);
    }
    // now set it so that our lower is at 0,0,0
    fvec3 lower_bound = bounds.lower;
    bounds.upper -= lower_bound;
    bounds.lower = fvec3(0,0,0);
    center_pos = bounds.get_center_pos();
    
    fvec3 aligned_center_pos = get_nearest_half_or_whole(center_pos);
    fvec3 center_offset = aligned_center_pos - center_pos;
    bounds.lower += center_offset;
    bounds.upper += center_offset;
    fvec3 offset = center_offset - lower_bound;
    
    for (unsigned int i = 0; i < model_vertices.size(); i++) {
        model_vertices[i] += offset;
    }
    
    bounds.lower = get_round_from_fvec3(bounds.lower);
    bounds.upper = get_round_from_fvec3(bounds.upper);
    center_pos = bounds.get_center_pos();
}

bool RenderableModel::model_keyboard_callback(Game* game, Entity* ent, ivec3 rwc, Action key) {
    if (key_bindings.count(key)) {
        return (*keyboard_callback)(game,
                                    ent,
                                    0,
                                    rwc,
                                    key);
    }
    return false;
}

bool RenderableModel::model_mouse_callback(Game* game, Entity* ent, Action key) {
    fvec4 lookingat;
    if (get_look_at_vehicle(&lookingat)) {
        return (*mouse_callback)(game,
                                 ent,
                                 0,
                                 fvec3(lookingat.x, lookingat.y, lookingat.z),
                                 key);
    }
    return false;
}