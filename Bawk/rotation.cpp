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

#define ANGLE_THRESHOLD 0.001f

Rotation::Rotation() {
    quaternion = glm::angleAxis(0.0f, fvec3(1,0,0));
    free_y = true;
}


void Rotation::set_free_y(bool fy) {
    free_y = fy;
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
    glm::quat inverse;
    inverse.w = -rounded_quaternion.w;
    inverse.x = rounded_quaternion.x;
    inverse.y = rounded_quaternion.y;
    inverse.z = rounded_quaternion.z;
    *mat = *mat * glm::mat4_cast(inverse);
}

void Rotation::add_world_rotation_rounded(fmat4* mat) {
    *mat = *mat * glm::mat4_cast(rounded_quaternion);
}

void Rotation::set_angle(glm::quat angle) {
    quaternion = angle;
    recalculate_rounded();
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

void Rotation::apply_angle(float angle, fvec3 axis) {
    // rotate our quaternion around the axis, by angle amount
    //axis = glm::normalize(axis);
    glm::quat temp = glm::angleAxis(angle, axis);
    quaternion = glm::cross(temp, quaternion);
    //quaternion = glm::rotate(quaternion, angle, axis);
    quaternion = glm::normalize(quaternion);
}

void Rotation::apply_angles(fvec3 angular_velocity) {
    fvec3 sangular = get_scaled_angle(angular_velocity);
    
    // apply (relative) yaw
    if (free_y) {
        apply_angle(sangular.x, get_up());
    }
    else {
        apply_angle(sangular.x, fvec3(0,1,0));
    }
    // recalculate right, dir is out of sync
    fvec3 right = get_right();
    // now apply (relative) pitch
    apply_angle(sangular.y, right);
    // recalculate dir, up is out of sync
    fvec3 dir = get_dir();
    // now apply (relative) roll
    apply_angle(sangular.z, dir);
    // recalculate right, up
    //rightq = glm::mat4_cast(quaternion) * fvec4(0,0,1,0);
    //right = get_fvec3_from_fvec4(rightq);
    //right = glm::normalize(right);
    
    //up = glm::cross(right, dir);
    //up = glm::normalize(up);
    
    recalculate_rounded();
}

void Rotation::recalculate_rounded() {
    // first, try to align dir to an axis
    fvec3 dir = get_dir();
    fvec3 rounded_dir = get_closest_rounded_vector(dir);
    float rounded_angle0 = acosf(glm::dot(rounded_dir, dir));
    fvec3 rounded_rotation_axis0, rounded_rotation_axis1;
    if (rounded_angle0 != 0) {
        rounded_rotation_axis0 = glm::cross(dir, rounded_dir);
        rounded_rotation_axis0 = glm::normalize(rounded_rotation_axis0);
    }
    else {
        rounded_rotation_axis0 = fvec3(0, 1, 0);
    }
    glm::quat temp = glm::angleAxis(rounded_angle0, rounded_rotation_axis0);
    rounded_quaternion = glm::cross(temp, quaternion);
    
    fvec4 rightq = glm::mat4_cast(rounded_quaternion) * fvec4(0,0,1,0);
    fvec3 temp_right = get_fvec3_from_fvec4(rightq);
    temp_right = glm::normalize(temp_right);
    
    // now align right to an axis
    fvec3 rounded_right = get_closest_rounded_vector(temp_right);
    float rounded_angle1 = acosf(glm::dot(rounded_right, temp_right));
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

void Rotation::set_to_point(fvec3 to_dir, fvec3 to_up) {
    // align dir to to_dir
    fvec3 dir = get_dir();
    float val = glm::dot(to_dir, dir);
    if (val > 1.0f) {
        // account for rounding errors...
        val = 1.0f;
    }
    else if (val < -1.0f) {
        val = -1.0f;
    }
    float to_angle0 = acosf(val);
    if (to_angle0 != 0) {
        fvec3 to_rotation_axis0 = glm::cross(dir, to_dir);
        if (to_rotation_axis0 == fvec3(0,0,0)) {
            // can rotate around any axis
            to_rotation_axis0 = get_up();
        }
        else {
            to_rotation_axis0 = glm::normalize(to_rotation_axis0);
        }
        apply_angle(to_angle0, to_rotation_axis0);
    }
    
    fvec3 temp_up = get_up();
    // now align up to an axis
    val = glm::dot(to_up, temp_up);
    if (val > 1.0f) {
        // account for rounding errors...
        val = 1.0f;
    }
    else if (val < -1.0f) {
        val = -1.0f;
    }
    float to_angle1 = acosf(val);
    if (to_angle1 != 0) {
        fvec3 to_rotation_axis1 = glm::cross(temp_up, to_up);
        if (to_rotation_axis1 == fvec3(0,0,0)) {
            // can rotate around any axis
            to_rotation_axis1 = to_dir;
        }
        else {
            to_rotation_axis1 = glm::normalize(to_rotation_axis1);
        }
        apply_angle(to_angle1, to_rotation_axis1);
    }
    recalculate_rounded();
}

void Rotation::inch_toward_normalization(float max_angle) {
    fvec3 up = get_up();
    fvec3 rounded_up = get_closest_rounded_vector(up);
    float rounded_angle0 = acosf(glm::dot(rounded_up, up));
    
    // TODO make this save state sort of, and not happen fully automatically
    if (rounded_angle0 < ANGLE_THRESHOLD) {
        
        fvec3 dir = get_dir();
        fvec3 rounded_dir = get_closest_rounded_vector(dir);
        float rounded_angle1 = acosf(glm::dot(rounded_dir, dir));
        if (rounded_angle1 < ANGLE_THRESHOLD) {
            // close enough
            return;
        }
        rounded_angle1 = bound_absolute(rounded_angle1, max_angle);
        fvec3 to_rotation_axis1 = glm::cross(dir, rounded_dir);
        if (to_rotation_axis1 == fvec3(0,0,0)) {
            // can rotate around any axis
            to_rotation_axis1 = get_dir();
        }
        else {
            to_rotation_axis1 = glm::normalize(to_rotation_axis1);
        }
        apply_angle(rounded_angle1, to_rotation_axis1);
    }
    else {
        rounded_angle0 = bound_absolute(rounded_angle0, max_angle);
        fvec3 to_rotation_axis0 = glm::cross(up, rounded_up);
        if (to_rotation_axis0 == fvec3(0,0,0)) {
            // can rotate around any axis
            to_rotation_axis0 = get_dir();
        }
        else {
            to_rotation_axis0 = glm::normalize(to_rotation_axis0);
        }
        apply_angle(rounded_angle0, to_rotation_axis0);
    }
}

void Rotation::transform_into_my_rotation(Rotation* dst, Rotation other) {
    glm::quat inverse;
    inverse.w = -rounded_quaternion.w;
    inverse.x = rounded_quaternion.x;
    inverse.y = rounded_quaternion.y;
    inverse.z = rounded_quaternion.z;

    glm::quat result = glm::cross(inverse, other.quaternion);
    dst->set_angle(result);
}

void Rotation::transform_into_world_rotation(Rotation* dst, Rotation other) {
    glm::quat result = glm::cross(quaternion, other.quaternion);
    dst->set_angle(result);
}

fvec3 Rotation::transform_point_into_my_rotation(fvec3 rwc) {
    fvec4 result(rwc.x, rwc.y, rwc.z, 1.0f);
    fmat4 mat(1);
    add_my_rotation_rounded(&mat);
    result = mat * result;
    fvec3 src(result.x,result.y,result.z);
    return src;
}
/*
fvec3 Rotation::transform_into_world_coordinates_smooth(fvec3 rwc) {
    fvec4 result(rwc.x, rwc.y, rwc.z, 1.0f);
    fmat4 mat(1);
    add_world_rotation(&mat);
    result = mat * result;
    fvec3 src(result.x,result.y,result.z);
    return src;
}*/

fvec3 Rotation::get_dir() {
    fvec4 dirq = glm::mat4_cast(quaternion) * fvec4(1,0,0,0);
    fvec3 dir = get_fvec3_from_fvec4(dirq);
    dir = glm::normalize(dir);
    return dir;
}

fvec3 Rotation::get_up() {
    fvec4 upq = glm::mat4_cast(quaternion) * fvec4(0,1,0,0);
    fvec3 up = get_fvec3_from_fvec4(upq);
    up = glm::normalize(up);
    return up;
}

fvec3 Rotation::get_right() {
    fvec4 rightq = glm::mat4_cast(quaternion) * fvec4(0,0,1,0);
    fvec3 right = get_fvec3_from_fvec4(rightq);
    right = glm::normalize(right);
    return right;
}

fvec3 Rotation::get_forward() {
    fvec3 dir = get_dir();
    fvec3 forward(dir.x, 0, dir.z);
    forward = glm::normalize(forward);
    return forward;
}