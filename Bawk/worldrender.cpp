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
#include "texture_allocator.h"

#include "opengl_debug.h"

namespace OGLAttr {
    // shaders and attributes set by shader loading program
    // put these inside a namespace or some shizzle
    GLuint vao;
    GLuint tile_texture;
    GLuint active_tile_texture;
    
    GLuint common_vertex_vbo;
    GLuint common_texture_vbo;
    
    GeometryShaderProgram geometry_shader;
    LightingShaderProgram lighting_shader;
    ShadowShaderProgram shadow_shader;
    
    FirstPassShaderProgram* current_shader;
}

using namespace OGLAttr;

// some local variables, these shouldn't be directly exposed
fmat4 camera_transform;

void ShaderProgram::set_transform_matrix(fmat4* view) {
    fmat4 mvp = camera_transform * (*view);
    check_for_error();
    glUniformMatrix4fv(this->mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    check_for_error();
}

void FirstPassShaderProgram::set_coord_attribute(GLenum type) {
    glVertexAttribPointer(coord, 3, type, GL_FALSE, 0, 0);
}

void GeometryShaderProgram::set_transform_matrix(fmat4* view) {
    fmat4 mvp = camera_transform * (*view);
    glUniformMatrix4fv(this->mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(world_transform, 1, GL_FALSE, glm::value_ptr(*view));
    check_for_error();
}

void GeometryShaderProgram::set_texture_coord_attribute(GLenum type) {
    glVertexAttribPointer(texture_coord, 3, type, GL_FALSE, 0, 0);
}

void GeometryShaderProgram::set_block_draw_mode(BlockDrawMode v) {
    glUniform1i(draw_mode, v);
    check_for_error();
}

void GeometryShaderProgram::set_shader_intensity(float m) {
    glUniform1f(intensity, m);
    //glUniform1f(geometry_intensity, 0.3f);
    check_for_error();
}

void GeometryShaderProgram::set_alpha_cutoff(float a) {
    glUniform1f(alphacutoff, a);
    check_for_error();
}


int world_load_resources() {
    check_for_error();
    
    active_tile_texture = reserve_n_active_textures(1);
    set_active_texture(active_tile_texture);
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
    
    check_for_error();
    return 0;
}

void world_free_resources() {
    free_game_info();
    delete_all_buffers();
    
    glDeleteTextures(1, &tile_texture);
    
    glDeleteBuffers(1, &common_vertex_vbo);
    glDeleteBuffers(1, &common_texture_vbo);
    check_for_error();
}

void set_geometry_as_current_shader() {
    glUseProgram(geometry_shader.program);
    glEnableVertexAttribArray(geometry_shader.coord);
    glEnableVertexAttribArray(geometry_shader.texture_coord);
    current_shader = &geometry_shader;
    check_for_error();
}

void set_shadow_as_current_shader() {
    glUseProgram(shadow_shader.program);
    glEnableVertexAttribArray(shadow_shader.coord);
    //glEnableVertexAttribArray(shadow_shader.texture_coord);
    current_shader = &shadow_shader;
    check_for_error();
}

void set_camera_transform_matrix(fmat4* camera) {
    camera_transform = *camera;
}

void set_unitary_transform_matrix() {
    glm::mat4 one(1);
    camera_transform = one;
    current_shader->set_transform_matrix(&one);
}

fvec4 apply_mvp_matrix(fmat4* view, fvec4 a) {
    fmat4 mvp = camera_transform * (*view);
    return mvp * a;
}

void set_lighting_block_draw_mode(LightDrawMode v) {
    glUniform1i(lighting_shader.draw_mode, v);
    check_for_error();
}

void set_lighting_val(fvec3 val) {
    glUniform3f(lighting_shader.val, val.x, val.y, val.z);
    check_for_error();
}

void set_lighting_properties(float light_radius, float light_intensity) {
    glUniform3f(lighting_shader.properties, light_radius, light_intensity, 0.0f);
    check_for_error();
}

void set_ambient_lighting_properties(float light_intensity) {
    glUniform3f(lighting_shader.properties, 0.0f, light_intensity, 0.0f);
}

void set_directional_lighting_transform_matrix(fmat4* light_mvp) {
    glUniformMatrix4fv(lighting_shader.shadow_mvp, 1, GL_FALSE, glm::value_ptr(*light_mvp));
}

void set_unitary_lighting_transform_matrix() {
    fmat4 one(1);
    glUniformMatrix4fv(lighting_shader.mvp, 1, GL_FALSE, glm::value_ptr(one));
    check_for_error();
}

void set_lighting_transform_matrix(fmat4* view) {
    fmat4 mvp = camera_transform * (*view);
    glUniformMatrix4fv(lighting_shader.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    check_for_error();
}

void set_lighting_screen_size(float width, float height) {
    glUniform2f(lighting_shader.screen_size, width, height);
    check_for_error();
}