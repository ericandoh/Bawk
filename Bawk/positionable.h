//
//  positionable.h
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__positionable__
#define __Bawk__positionable__

#include <stdio.h>
#include "basic_types.h"
#include "rotation.h"

class Positionable {
protected:
    // matrices
    //fmat4 into_world_mat;
    //fmat4 into_world_mat_smooth;
    //fmat4 into_my_mat;
    //fmat4 into_my_mat_smooth;
    // --- POSITIONING ---
    // position of the superobject, in parent's coordinate system
    fvec3 pos;
    
    // angle around dir, in parent's coordinate system
    Rotation angle;
    
    // the center position of this entity. usually set to the middle point of the bounding box, in OAC
    fvec3 center_pos;
public:
    // mark this if we enable rotations for this object
    bool can_rotate;
    
    Positionable();
    
    // internal function to transform RWC xyz to OAC src
    virtual void transform_into_my_coordinates(fvec3* src, float x, float y, float z);
    virtual void transform_into_my_integer_coordinates(ivec3* src, int x, int y, int z);
    virtual void transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z);
    // internal function to transform OAC xyz to RWC src
    virtual void transform_into_world_coordinates(fvec3* src, float x, float y, float z);
    virtual void transform_into_world_integer_coordinates(ivec3* src, int x, int y, int z);
    virtual void transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z);
    
    virtual void transform_into_my_rotation(Rotation* dst, Rotation target);
    virtual void transform_into_world_rotation(Rotation* dst, Rotation target);
    
    
    void get_mvp(fmat4* dst);
    
    void set_pos(fvec3 p);
    Rotation* get_angle();
    void set_angle(Rotation a);
    void set_pos_and_angle(fvec3 p, Rotation a);
    
    // these are relative to global
    virtual fvec3 get_world_pos();
    virtual Rotation get_world_rotation();
    fvec3 get_center_offset();
    
    
    void set_into_coordinate_of(Positionable* parent);
    
    // --- RENDER ---
    virtual void render(fmat4* transform) EMPTY_FUNCTION;
    virtual void render_lights(fmat4* transform, fvec3 player_pos) EMPTY_FUNCTION;
};

#endif /* defined(__Bawk__positionable__) */
