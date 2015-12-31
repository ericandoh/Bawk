//
//  texture_allocator.cpp
//  Bawk
//
//  Created by Eric Oh on 12/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "texture_allocator.h"
#include "importopengl.h"

int textures_allocated = 0;

int reserve_n_active_textures(int n) {
    int begin = textures_allocated;
    textures_allocated += n;
    return begin;
}

void set_active_texture(int n) {
    glActiveTexture(GL_TEXTURE0 + n);
}