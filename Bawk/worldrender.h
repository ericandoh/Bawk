//
//  Contains and loads/frees assets related to rendering the world
//  - OpenGL functions here
//  - Has assets global to rendering (not specific world rendering items)
//  - Actual world rendering is done at chunk-level, which will use assets from this class
//    (this is to preserve hierarchy)
//  - Loads/Requests resources at beginning, and frees them at end
//
//  Used by:
//  - world.cpp
//
//  Uses:
//  - OpenGL library
//
//  Notes
//  - If we end up switching to another rendering program or using a voxel engine we have to
//    change this class only
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__worldrender__
#define __Bawk__worldrender__

#include <stdio.h>
#include <GLFW/glfw3.h>
#include "basic_types.h"
#include "superobject.h"

extern GLuint block_attribute_coord;
extern GLuint texture_attribute_coord;
extern GLuint block_uniform_mvp;
extern GLuint program;

int world_load_resources();
void world_free_resources();
void set_transform_matrix(fmat4 mvp);

#endif /* defined(__Bawk__worldrender__) */