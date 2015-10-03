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
#include "worldrender.h"

// shaders and attributes set by shader loading program
GLuint block_attribute_coord;
GLuint texture_attribute_coord;
GLuint block_uniform_mvp;
GLuint program;

int world_load_resources() {
    if (set_shaders(&block_attribute_coord,
                    &texture_attribute_coord,
                    &block_uniform_mvp,
                    &program)) {
        return -1;
    }
    
    glEnable(GL_CULL_FACE);
    glPolygonOffset(1, 1);
    return 0;
}

void world_free_resources() {
    glDeleteProgram(program);
}

void set_transform_matrix(fmat4 mvp) {
    glUniformMatrix4fv(block_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}