//
//  rotation.cpp
//  Bawk
//
//  fuck rotation
//
//  Created by Eric Oh on 11/14/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "rotation.h"

Rotation::Rotation() {
    up = fvec3(0,1,0);
    dir = fvec3(1,0,0);
    right = fvec3(0,0,1);
    rotation_state = fvec4(1,0,0,0);
    quaternion = glm::angleAxis(0.0f, fvec3(0,1,0));
    apply_angles(fvec3(0,0,0));
    free_y = true;
}

void Rotation::add_my_rotation(fmat4* mat) {
    glm::quat inverse;
    inverse.w = -quaternion.w;
    inverse.x = quaternion.x;
    inverse.y = quaternion.y;
    inverse.z = quaternion.z;
    *mat = *mat * glm::mat4_cast(inverse);
}

void Rotation::add_world_rotation(fmat4* mat) {
    *mat = *mat * glm::mat4_cast(quaternion);
}

void Rotation::add_my_rotation_rounded(fmat4* mat) {
    // extract info from my current quaternion
    /*fvec3 rot_axis;
    float half_rotation_angle = acosf(quaternion.w);
    float rotation_angle_rounded = roundf(half_rotation_angle / HALF_PI * 2)*HALF_PI;
    rot_axis.x = quaternion.x / sinf(half_rotation_angle);
    rot_axis.y = quaternion.y / sinf(half_rotation_angle);
    rot_axis.z = quaternion.z / sinf(half_rotation_angle);
    glm::quat rounded = glm::angleAxis(rotation_angle_rounded, rot_axis);*/
    glm::quat inverse;
    inverse.w = -rounded_quaternion.w;
    inverse.x = rounded_quaternion.x;
    inverse.y = rounded_quaternion.y;
    inverse.z = rounded_quaternion.z;
    *mat = *mat * glm::mat4_cast(inverse);
}

void Rotation::add_world_rotation_rounded(fmat4* mat) {
    // extract info from my current quaternion
    /*fvec3 rot_axis;
    float half_rotation_angle = acosf(quaternion.w);
    float rotation_angle_rounded = roundf(half_rotation_angle / HALF_PI * 2)*HALF_PI;
    rot_axis.x = quaternion.x / sinf(half_rotation_angle);
    rot_axis.y = quaternion.y / sinf(half_rotation_angle);
    rot_axis.z = quaternion.z / sinf(half_rotation_angle);
    glm::quat rounded = glm::angleAxis(-rotation_angle_rounded, rot_axis);*/
    *mat = *mat * glm::mat4_cast(rounded_quaternion);
}

fvec3 get_scaled_angle(fvec3 angle) {
    return fvec3(angle.x / RIGHT_ANGLE * HALF_PI,
                 angle.y / RIGHT_ANGLE * HALF_PI,
                 angle.z / RIGHT_ANGLE * HALF_PI);
}

fvec3 get_rounded_angle(fvec3 angle) {
    return fvec3(roundf(angle.x / HALF_PI) * HALF_PI,
                 roundf(angle.y / HALF_PI) * HALF_PI,
                 roundf(angle.z / HALF_PI) * HALF_PI);
}

fvec3 get_closest_rounded_vector(fvec3 from) {
    if (fabsf(from.x) > fabsf(from.y)) {
        if (fabsf(from.x) > fabsf(from.z)) {
            // use x
            return fvec3(roundf(from.x),0,0);
        }
        else {
            // use z
            return fvec3(0,0,roundf(from.z));
        }
    }
    else if (fabsf(from.z) > fabsf(from.y)) {
        // use z
        return fvec3(0,0,roundf(from.z));
    }
    else {
        // use y
        return fvec3(0,roundf(from.y),0);
    }
}

void Rotation::apply_angle(float angle, fvec3 axis) {
    // rotate our quaternion around the axis, by angle amount
    //axis = glm::normalize(axis);
    glm::quat temp = glm::angleAxis(angle, axis);
    quaternion = glm::cross(temp, quaternion);
    //quaternion = glm::rotate(quaternion, angle, axis);
    quaternion = glm::normalize(quaternion);
}

void Rotation::apply_angles(fvec3 angular_velocity) {
    rotation_state.x = quaternion.x;
    rotation_state.y = quaternion.y;
    rotation_state.z = quaternion.z;
    rotation_state.w = quaternion.w;
    
    fvec3 sangular = get_scaled_angle(angular_velocity);
    
    // apply (relative) yaw
    if (free_y) {
        apply_angle(sangular.x, up);
    }
    else {
        apply_angle(sangular.x, fvec3(0,1,0));
    }
    // recalculate right, dir is out of sync
    fvec4 rightq = glm::mat4_cast(quaternion) * fvec4(0,0,1,0);
    right.x = rightq.x;
    right.y = rightq.y;
    right.z = rightq.z;
    right = glm::normalize(right);
    // now apply (relative) pitch
    apply_angle(sangular.y, right);
    // recalculate dir, up is out of sync
    fvec4 dirq = glm::mat4_cast(quaternion) * fvec4(1,0,0,0);
    dir.x = dirq.x;
    dir.y = dirq.y;
    dir.z = dirq.z;
    dir = glm::normalize(dir);
    // now apply (relative) roll
    apply_angle(sangular.z, dir);
    // recalculate right, up
    rightq = glm::mat4_cast(quaternion) * fvec4(0,0,1,0);
    right.x = rightq.x;
    right.y = rightq.y;
    right.z = rightq.z;
    right = glm::normalize(right);
    
    up = glm::cross(right, dir);
    up = glm::normalize(up);
    
    recalculate_rounded();
}

void Rotation::recalculate_rounded() {
    forward.x = dir.x;
    forward.y = 0;
    forward.z = dir.z;
    forward = glm::normalize(forward);
    
    // first, try to align dir to an axis
    fvec3 rounded_dir = get_closest_rounded_vector(dir);
    rounded_angle0 = acosf(glm::dot(rounded_dir, dir));
    if (rounded_angle0 != 0) {
        rounded_rotation_axis0 = glm::cross(dir, rounded_dir);
        rounded_rotation_axis0 = glm::normalize(rounded_rotation_axis0);
    }
    else {
        rounded_rotation_axis0 = fvec3(0, 1, 0);
    }
    glm::quat temp = glm::angleAxis(rounded_angle0, rounded_rotation_axis0);
    rounded_quaternion = glm::cross(temp, quaternion);
    
    fvec3 temp_right;
    fvec4 rightq = glm::mat4_cast(rounded_quaternion) * fvec4(0,0,1,0);
    temp_right.x = rightq.x;
    temp_right.y = rightq.y;
    temp_right.z = rightq.z;
    temp_right = glm::normalize(temp_right);
    
    // now align right to an axis
    fvec3 rounded_right = get_closest_rounded_vector(temp_right);
    rounded_angle1 = acosf(glm::dot(rounded_right, temp_right));
    if (rounded_angle1 != 0) {
        rounded_rotation_axis1 = glm::cross(temp_right, rounded_right);
        rounded_rotation_axis1 = glm::normalize(rounded_rotation_axis1);
    }
    else {
        rounded_rotation_axis1 = fvec3(0, 1, 0);
    }
    temp = glm::angleAxis(rounded_angle1, rounded_rotation_axis1);
    rounded_quaternion = glm::cross(temp, rounded_quaternion);
    rounded_quaternion = glm::normalize(rounded_quaternion);
}

void Rotation::set_angle(glm::quat angle) {
    quaternion = angle;
    
    // recalculate right
    fvec4 rightq = glm::mat4_cast(quaternion) * fvec4(0,0,1,0);
    right.x = rightq.x;
    right.y = rightq.y;
    right.z = rightq.z;
    right = glm::normalize(right);
    // recalculate dir
    fvec4 dirq = glm::mat4_cast(quaternion) * fvec4(1,0,0,0);
    dir.x = dirq.x;
    dir.y = dirq.y;
    dir.z = dirq.z;
    dir = glm::normalize(dir);
    
    up = glm::cross(right, dir);
    up = glm::normalize(up);

    recalculate_rounded();
}

void Rotation::reset_rotation() {
    quaternion.x = rotation_state.x;
    quaternion.y = rotation_state.y;
    quaternion.z = rotation_state.z;
    quaternion.w = rotation_state.w;
    set_angle(quaternion);
}

void Rotation::inch_toward_normalization() {
    // TODO make this save state sort of, and not happen fully automatically
    if (rounded_angle0 < 0.001f && rounded_angle1 < 0.001f) {
        // close enough
        return;
    }
    float move_angle0 = rounded_angle0;
    float move_angle1 = rounded_angle1;
    float max_move = 0.04f;
    
    if (move_angle0 < -max_move) {
        move_angle0 = -max_move;
    }
    if (move_angle0 > max_move) {
        move_angle0 = max_move;
    }
    if (move_angle1 < -max_move) {
        move_angle1 = -max_move;
    }
    if (move_angle1 > max_move) {
        move_angle1 = max_move;
    }
    
    glm::quat temp0 = glm::angleAxis(move_angle0, rounded_rotation_axis0);
    glm::quat temp1 = glm::angleAxis(move_angle1, rounded_rotation_axis1);
    quaternion = glm::cross(temp1, glm::cross(temp0, quaternion));
}

void Rotation::transform_into_my_rotation(Rotation dst, Rotation other) {
    glm::quat inverse;
    inverse.w = -rounded_quaternion.w;
    inverse.x = rounded_quaternion.x;
    inverse.y = rounded_quaternion.y;
    inverse.z = rounded_quaternion.z;

    glm::quat result = glm::cross(inverse, other.quaternion);
    dst.set_angle(result);
}

void Rotation::transform_into_world_rotation(Rotation dst, Rotation other) {
    glm::quat result = glm::cross(quaternion, other.quaternion);
    dst.set_angle(result);
}