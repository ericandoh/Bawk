//
//  directionallightrender.cpp
//  Bawk
//
//  Created by Eric Oh on 12/24/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "directionallightrender.h"
#include "display.h"
#include "worldrender.h"
#include <glm/gtc/matrix_transform.hpp>

#define SUN_HEIGHT 50.0f

DirectionalRenderableLight::DirectionalRenderableLight() {
    direction = fvec3(1, 2, 3);
    direction = glm::normalize(direction);
}

void DirectionalRenderableLight::render_light() {
    // TODO implement
}

fvec3 get_height_aligned_offset(fvec3 src_pos, fvec3 dir, float to_height) {
    if (dir.y == 0.0f) {
        // edge case
        return fvec3(0.0f, to_height - src_pos.y, 0.0f);
    }
    // add some x of dir to src_pos so that new_src_pos.y = to_height
    float dir_steps = (to_height - src_pos.y) / dir.y;
    fvec3 result = (dir_steps * dir);
    return result;
}

void DirectionalRenderableLight::set_camera(fvec3 player_pos) {
    int width, height;
    get_window_size(&width, &height);
    
    float length = 50.0f;
    fvec3 center_pos = player_pos; // TODO + player->dir * FAR * 0.5f;
    float sun_height = std::max(SUN_HEIGHT, SUN_HEIGHT + center_pos.y);
    fvec3 start_pos = get_height_aligned_offset(center_pos, direction, sun_height) + center_pos;
    fvec3 toward_pos = center_pos;
    
    fmat4 view = glm::lookAt(start_pos, toward_pos, fvec3(0, 1, 0));
    
    // left, right, bottom, top
    fmat4 projection = glm::ortho(-length, length, -length, length, 0.01f, 100.0f);
    mvp = projection * view;
    
    set_camera_transform_matrix(&mvp);
}

void DirectionalRenderableLight::set_camera_for_light(fvec3 player_pos) {
    fmat4 bias_mat(0.5, 0.0, 0.0, 0.0,
                   0.0, 0.5, 0.0, 0.0,
                   0.0, 0.0, 0.5, 0.0,
                   0.5, 0.5, 0.5, 1.0);
    
    fmat4 result = bias_mat * mvp;
    set_directional_lighting_transform_matrix(&result);
}
