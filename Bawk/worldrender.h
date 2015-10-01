//
//  All rendering logic for the world is stored in this object
//  - Encapsulates OpenGL here
//  - Stores relevant chunk meshes to render
//  - Loads/Requests resources at beginning, and frees them at end
//  -
//
//  Used by:
//  - world.cpp
//
//  Uses:
//  - OpenGL library
//  -
//  -
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
//removeme
#include "chunk.h"

extern GLuint block_attribute_coord;
extern GLuint block_uniform_mvp;
extern GLuint program;

class WorldRender {
    Chunk chunk;
public:
    WorldRender();
    int load_resources();
    void render();
    void free_resources();
};

#endif /* defined(__Bawk__worldrender__) */