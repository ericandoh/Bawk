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
    fmat4 into_world_mat;
    fmat4 into_world_mat_smooth;
    fmat4 into_my_mat;
    fmat4 into_my_mat_smooth;
public:
    // mark this if we enable rotations for this object
    bool can_rotate;
    
    // --- POSITIONING ---
    // position of the superobject, in RWC
    fvec3 pos;
    
    // angle around dir
    Rotation angle;
    
    // the center position of this entity. usually set to the middle point of the bounding box
    fvec3 center_pos;
    
    Positionable();
    
    // internal function to transform RWC xyz to OAC src
    void transform_into_my_coordinates(fvec3* src, float x, float y, float z);
    void transform_into_my_integer_coordinates(ivec3* src, int x, int y, int z);
    void transform_into_my_coordinates_smooth(fvec3* src, float x, float y, float z);
    // internal function to transform OAC xyz to RWC src
    void transform_into_world_coordinates(fvec3* src, float x, float y, float z);
    void transform_into_world_integer_coordinates(ivec3* src, int x, int y, int z);
    void transform_into_world_coordinates_smooth(fvec3* src, float x, float y, float z);
    
    void get_mvp(fmat4* dst);
    
    void set_pos(fvec3 p);
    void set_angle(Rotation a);
    
    virtual void recalculate_transform();
    
    // --- RENDER ---
    virtual void render(fmat4* transform) EMPTY_FUNCTION;
    virtual void render_lights(fmat4* transform, fvec3 player_pos) EMPTY_FUNCTION;
};

#endif /* defined(__Bawk__positionable__) */
