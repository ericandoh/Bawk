//
//  File containing some basic structures like position, etc
//  -
//  -
//  -
//
//  Used by:
//  - A lot of shit
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  - Can later set typedef (OPENGL class) Position
//  - for now unused, later convert all glm::vec3 to above
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_math_h
#define Bawk_math_h

struct Position {
    float x, y, z;
    Position(float ix=0, float iy=0, float iz=0): x(ix), y(iy), z(iz){}
};

struct Direction {
    float x, y, z;
    Direction(float ix=0, float iy=0, float iz=0): x(ix), y(iy), z(iz){}
};

#endif