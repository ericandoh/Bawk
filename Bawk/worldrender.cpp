//
//  worldrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.h"
#include "worldrender.h"
#include "chunkrender.h"
#include "cursorblock.h"
#include "game_info_loader.h"

// shaders and attributes set by shader loading program
// put these inside a namespace or some shizzle
GLuint vao;

GLuint geometry_program;
GLuint lighting_program;

GLuint geometry_coord;
GLuint geometry_texture_coord;
GLuint geometry_mvp;
GLuint geometry_world_transform;
GLuint geometry_draw_mode;
GLuint geometry_intensity;
GLuint geometry_alphacutoff;
GLuint geometry_tile_texture;

GLuint lighting_coord;
GLuint lighting_mvp;

GLuint lighting_position_map;
GLuint lighting_color_map;
GLuint lighting_color_t_map;
GLuint lighting_normal_map;

GLuint lighting_screen_size;
GLuint lighting_val;
GLuint lighting_draw_mode;

GLuint tile_texture;

int CHUNK_RENDER_DIST = 3;

GLuint common_vertex_vbo;
GLuint common_texture_vbo;

void check_for_error_w() {
    int error = glGetError();
    if (error != 0) {
        printf("Error in OPENGL: %d\n",error);
    }
}

int world_load_resources() {
    check_for_error_w();
    glActiveTexture(GL_TEXTURE0);
    // Load textures
    tile_texture = load_tiles();
    glBindTexture(GL_TEXTURE_2D, tile_texture);
    
    printf("Loaded tile texture\n");
    
    glEnable(GL_CULL_FACE);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_NEAREST_MIPMAP_LINEAR if you want to use mipmaps
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glPolygonOffset(1, 1);
    //glEnable (GL_BLEND);
    //glDisable(GL_BLEND);
    
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glGenBuffers(1, &common_vertex_vbo);
    glGenBuffers(1, &common_texture_vbo);
    
    load_game_info();
    
    check_for_error_w();
    return 0;
}

void world_free_resources() {
    free_game_info();
    delete_all_buffers();
    
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
    glUniform1i(geometry_draw_mode, v);
}

void set_unitary_transform_matrix() {
    glm::mat4 one(1);
    glUniformMatrix4fv(geometry_mvp, 1, GL_FALSE, glm::value_ptr(one));
    glUniformMatrix4fv(geometry_world_transform, 1, GL_FALSE, glm::value_ptr(one));
}

void set_transform_matrix(fmat4 mvp, fmat4 view) {
    glUniformMatrix4fv(geometry_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(geometry_world_transform, 1, GL_FALSE, glm::value_ptr(view));
}

void set_shader_intensity(float m) {
    //glUniform1f(geometry_intensity, m);
    glUniform1f(geometry_intensity, 1.0f);
}

void set_alpha_cutoff(float a) {
    glUniform1f(geometry_alphacutoff, a);
}

void set_lighting_block_draw_mode(int v) {
    glUniform1i(lighting_draw_mode, v);
}
void set_lighting_transform_matrix(fmat4 mvp) {
    glUniformMatrix4fv(lighting_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void set_lighting_screen_size(float width, float height) {
    glUniform2f(lighting_screen_size, width, height);
}