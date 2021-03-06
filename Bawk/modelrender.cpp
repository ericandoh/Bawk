//
//  modelrender.cpp
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelrender.h"

RenderableModel::RenderableModel() {
    name = "";
    resistance = 0;
    weight = 0;
    vehicle = 0;
    bounds = bounding_box();
    center_pos = fvec3(0,0,0);
    light = RenderableLight();
    multiplexer = 0;
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
    center_pos = bounds.get_world_pos();
    
    fvec3 aligned_center_pos = get_nearest_half_or_whole(center_pos);
    if (aligned_center_pos.x < 0.5f) {
        aligned_center_pos.x = 0.5f;
    }
    if (aligned_center_pos.y < 0.5f) {
        aligned_center_pos.y = 0.5f;
    }
    if (aligned_center_pos.z < 0.5f) {
        aligned_center_pos.z = 0.5f;
    }
    fvec3 center_offset = aligned_center_pos - center_pos;
    bounds.lower += center_offset;
    bounds.upper += center_offset;
    fvec3 offset = center_offset - lower_bound;
    
    for (unsigned int i = 0; i < model_vertices.size(); i++) {
        model_vertices[i] += offset;
    }
    
    bounds.lower = get_round_from_fvec3(bounds.lower);
    bounds.upper = get_round_from_fvec3(bounds.upper);
    // nothing should be 0 wide...
    if (bounds.upper.x == 0) {
        bounds.upper.x = 1.0f;
    }
    if (bounds.upper.y == 0) {
        bounds.upper.y = 1.0f;
    }
    if (bounds.upper.z == 0) {
        bounds.upper.z = 1.0f;
    }
    center_pos = bounds.get_world_pos();
}