//
//  positionable.cpp
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "positionable.h"

Positionable::Positionable() {
    can_rotate = false;
    
    // set initial pos/angle to origin, overwrite for most objects
    pos = fvec3(0,0,0);
    angle = Rotation();
    center_pos = fvec3(0,0,0);
}

void Positionable::transform_into_my_coordinates(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_my_mat * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Positionable::transform_into_my_integer_coordinates(ivec3* src, int x, int y, int z) {
    fvec3 result;
    transform_into_my_coordinates(&result, x + 0.5f, y + 0.5f, z + 0.5f);
    src->x = floorf(result.x);
    src->y = floorf(result.y);
    src->z = floorf(result.z);
}

void Positionable::transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_my_mat_smooth * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Positionable::transform_into_world_coordinates(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_world_mat * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Positionable::transform_into_world_integer_coordinates(ivec3* src, int x, int y, int z) {
    fvec3 result;
    transform_into_world_coordinates(&result, x + 0.5f, y + 0.5f, z + 0.5f);
    src->x = floorf(result.x);
    src->y = floorf(result.y);
    src->z = floorf(result.z);
}

void Positionable::transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z) {
    fvec4 result(x, y, z, 1.0f);
    result = into_world_mat_smooth * result;
    
    src->x = result.x;
    src->y = result.y;
    src->z = result.z;
}

void Positionable::get_mvp(fmat4 *dst) {
    *dst = into_world_mat_smooth;
}

void Positionable::set_pos(fvec3 p) {
    pos = p;
    recalculate_transform();
}

void Positionable::set_angle(Rotation a) {
    angle = a;
    recalculate_transform();
}

void Positionable::recalculate_transform() {
    // into my coordinates
    into_my_mat = fmat4(1);
    if (can_rotate) {
        into_my_mat = glm::translate(into_my_mat, center_pos);
        angle.add_my_rotation_rounded(&into_my_mat);
        into_my_mat = glm::translate(into_my_mat, -center_pos);
    }
    into_my_mat = glm::translate(into_my_mat, -pos);
    
    into_my_mat_smooth = fmat4(1);
    if (can_rotate) {
        into_my_mat_smooth = glm::translate(into_my_mat_smooth, center_pos);
        angle.add_my_rotation(&into_my_mat_smooth);
        into_my_mat_smooth = glm::translate(into_my_mat_smooth, -center_pos);
    }
    into_my_mat_smooth = glm::translate(into_my_mat_smooth, -pos);
    
    // into world coordinates
    into_world_mat = glm::translate(fmat4(1), pos);
    if (can_rotate) {
        // round angle to nearest angle
        into_world_mat = glm::translate(into_world_mat, center_pos);
        angle.add_world_rotation_rounded(&into_world_mat);
        into_world_mat = glm::translate(into_world_mat, -center_pos);
    }
    
    // into world coordinates, smoothly (without rounded angles)
    into_world_mat_smooth = glm::translate(fmat4(1), pos);
    if (can_rotate) {
        into_world_mat_smooth = glm::translate(into_world_mat_smooth, center_pos);
        angle.add_world_rotation(&into_world_mat_smooth);
        into_world_mat_smooth = glm::translate(into_world_mat_smooth, -center_pos);
    }
}
