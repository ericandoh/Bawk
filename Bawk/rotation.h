//
//  rotation.h
//  Bawk
//
//  Created by Eric Oh on 11/14/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__rotation__
#define __Bawk__rotation__

#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "basic_types.h"

#define RIGHT_ANGLE 90
#define STRAIGHT_ANGLE 180
#define FULL_ANGLE 360
const float HALF_PI = M_PI/2.0f;
const float TWO_PI = M_PI*2.0f;

class Rotation {
    fvec4 rotation_state;
    //fvec3 former_angle;
    float rounded_angle0, rounded_angle1;
    fvec3 rounded_rotation_axis0, rounded_rotation_axis1;
    void recalculate_rounded();
public:
    bool free_y;
    fvec3 angle;
    glm::quat quaternion;
    glm::quat rounded_quaternion;
    fvec3 dir, up, right;
    fvec3 forward;
    Rotation();
    void add_my_rotation(fmat4* mat);
    void add_world_rotation(fmat4* mat);
    void add_my_rotation_rounded(fmat4* mat);
    void add_world_rotation_rounded(fmat4* mat);
    
    void apply_angle(float angle, fvec3 axis);
    void apply_angles(fvec3 angular_velocity);
    void set_angle(glm::quat angle);
    void set_to_point(fvec3 to_dir, fvec3 to_up);
    void reset_rotation();
    void inch_toward_normalization();
    
    void transform_into_my_rotation(Rotation dst, Rotation other);
    void transform_into_world_rotation(Rotation dst, Rotation other);
};

#endif /* defined(__Bawk__rotation__) */
