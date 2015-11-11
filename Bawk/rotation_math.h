//
//  rotation_math.h
//  Bawk
//
//  Created by Eric Oh on 11/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__rotation_math__
#define __Bawk__rotation_math__

#include <stdio.h>
#include "basic_types.h"


/*
 * A rotation represents a transformation of some points around the zero-center
 * The rotation is integral - that is, unlike other rotation schemes which are 
 * fractional parts of PI, this rotation will be constricted to 90/180/270 degree
 * rotations.
 * The char x,y,z are either 0 or 1, and encode where (0,0,0) would go after the rotation
 * has been applied, given we rotate around (0.5,0.5,0.5).
 *
 * This rotation can be used as a form of state to hold rounded-orientations about 
 * objects in space
 *
 */
struct rotation {
    char x,y,z;
};

void convert_rotation_to_angles(fvec3* dst, rotation rot);
void convert_angles_to_rotation(rotation* dst, fvec3 angles);
// applies rotation to point, around zero
ivec3 convert_point_around_rotation(rotation rot, ivec3 point);

#endif /* defined(__Bawk__rotation_math__) */
