//
//  bresenham3d.h
//  Bawk
//
//  Created by Eric Oh on 10/24/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_bresenham3d_h
#define Bawk_bresenham3d_h

#include <stdio.h>
#include "basic_types.h"    //for BlockOrientation

// exposed for testing
fvec3 find_intersection_with_unit_cube(fvec3 point, fvec3 dir, BlockOrientation* orientation);

class BresenhamTracer {
public:
    void bresenham3D(float x1, float y1, float z1, const float x2, const float y2, const float z2);
    virtual bool check_at_coord(float x, float y, float z, BlockOrientation side) = 0;
};

#endif
