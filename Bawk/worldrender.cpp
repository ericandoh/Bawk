//
//  worldrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include "shader_loader.h"
#include "texture_loader.h"
#include "worldrender.h"
#include "chunkrender.h"
#include "cursorblock.h"
#include "game_info_loader.h"

// shaders and attributes set by shader loading program
GLuint block_attribute_coord;
GLuint texture_attribute_coord;
GLuint block_uniform_mvp;
GLuint block_uniform_draw_mode;
GLuint tile_texture;
GLuint program;

static GLint uniform_texture;

int CHUNK_RENDER_DIST = 3;
float BLOCK_PLACE_DST = 10.0f;

int mx, my, mz;
BlockOrientation face;
bool in_range;


GLuint common_vertex_vbo;
GLuint common_texture_vbo;

int world_load_resources() {
    if (set_shaders(&block_attribute_coord,
                    &texture_attribute_coord,
                    &block_uniform_mvp,
                    &block_uniform_draw_mode,
                    &program)) {
        return 1;
    }
    glActiveTexture(GL_TEXTURE0);
    // Load textures
    tile_texture = load_tiles();
    glBindTexture(GL_TEXTURE_2D, tile_texture);
    
    glUniform1i(uniform_texture, 0);
    
    glEnable(GL_CULL_FACE);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_NEAREST_MIPMAP_LINEAR if you want to use mipmaps
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glPolygonOffset(1, 1);
    
    mx = my = mz = 0;
    in_range = false;
    
    set_up_for_world_render();
    
    glGenBuffers(1, &common_vertex_vbo);
    glGenBuffers(1, &common_texture_vbo);
    
    load_game_info();
    
    return 0;
}

void world_free_resources() {
    free_game_info();
    delete_all_buffers();
    glDeleteProgram(program);
    glDeleteTextures(1, &tile_texture);
    
    glDeleteBuffers(1, &common_vertex_vbo);
    glDeleteBuffers(1, &common_texture_vbo);
}

GLuint get_vertex_attribute_vbo() {
    return common_vertex_vbo;
}

GLuint get_texture_attribute_vbo() {
    return common_texture_vbo;
}

void set_block_draw_mode(int v) {
    glUniform1i(block_uniform_draw_mode, v);
}

void set_transform_matrix(fmat4 mvp) {
    glUniformMatrix4fv(block_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void set_look_at(float depth, int x, int y, int z, BlockOrientation side) {
    in_range = depth <= BLOCK_PLACE_DST;
    if (!in_range) {
        return;
    }
    mx = x;
    my = y;
    mz = z;
    face = side;
}

bool get_look_at(ivec4* src) {
    if (in_range) {
        src->x = mx;
        src->y = my;
        src->z = mz;
        src->w = face;
    }
    return in_range;
}

void set_up_for_world_render() {
    // lets us know to use this program shaders
    glUseProgram(program);
}