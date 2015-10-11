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
extern GLuint block_uniform_draw_mode;
extern GLuint tile_texture;
extern GLuint program;

extern int CHUNK_RENDER_DIST;

int world_load_resources();
void world_free_resources();
void set_block_draw_mode(int v);
void set_transform_matrix(fmat4 mvp);
void set_look_at(float depth, int x, int y, int z, int side);
bool get_look_at(ivec4* src);
void set_up_for_world_render();

#endif /* defined(__Bawk__worldrender__) */