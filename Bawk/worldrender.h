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
#include "includeglfw.h"
#include "basic_types.h"
#include "superobject.h"
#include "block.h"

extern GLuint vao;

extern GLuint geometry_program;
extern GLuint lighting_program;

extern GLuint geometry_coord;
extern GLuint geometry_texture_coord;
extern GLuint geometry_mvp;
extern GLuint geometry_world_transform;
extern GLuint geometry_draw_mode;
extern GLuint geometry_intensity;
extern GLuint geometry_tile_texture;

extern GLuint lighting_coord;
extern GLuint lighting_mvp;
extern GLuint lighting_position_map;
extern GLuint lighting_color_map;
extern GLuint lighting_color_t_map;
extern GLuint lighting_normal_map;
extern GLuint lighting_screen_size;
extern GLuint lighting_val;
extern GLuint lighting_draw_mode;

extern GLuint tile_texture;

extern int CHUNK_RENDER_DIST;

int world_load_resources();
void world_free_resources();
GLuint get_vertex_attribute_vbo();
GLuint get_texture_attribute_vbo();
void set_block_draw_mode(int v);
void set_transform_matrix(fmat4 mvp);
void set_shader_intensity(float m);

#endif /* defined(__Bawk__worldrender__) */