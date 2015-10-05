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

// shaders and attributes set by shader loading program
GLuint block_attribute_coord;
GLuint texture_attribute_coord;
GLuint block_uniform_mvp;
GLuint tile_texture;
GLuint program;

static GLint uniform_texture;

int CHUNK_RENDER_DIST = 3;

int mx, my, mz, face;

int world_load_resources() {
    if (set_shaders(&block_attribute_coord,
                    &texture_attribute_coord,
                    &block_uniform_mvp,
                    &program)) {
        return -1;
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
    
    return 0;
}

void world_free_resources() {
    delete_all_buffers();
    glDeleteProgram(program);
    glDeleteTextures(1, &tile_texture);
}

void set_transform_matrix(fmat4 mvp) {
    glUniformMatrix4fv(block_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void set_look_at(int x, int y, int z, int side) {
    mx = x;
    my = y;
    mz = z;
    face = side;
}

ivec4 get_look_at() {
    return ivec4(mx, my, mz, face);
}