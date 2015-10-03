//
//  Represents a chunk
//  -
//  -
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  - Stolen shamelessly from https://en.wikibooks.org/wiki/OpenGL_Programming/Glescraft_1
//  -
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_chunk_h
#define Bawk_chunk_h

#include <GLFW/glfw3.h>

// dimensions of our chunk
#define CX 16
#define CY 16
#define CZ 16

class RenderableChunk {
    GLuint coord_vbo;           // VBO (vertex buffer object) for coordinates
    GLuint texture_vbo;         // VBO (vertex buffer object) for textures/flags
    int elements;               // number of blocks that need rendering
protected:
    uint16_t blk[CX][CY][CZ];   // blk type for each block
    bool changed;               // if chunk is updated, set this
public:
    RenderableChunk();
    ~RenderableChunk();
    void update();
    void render();
};

#endif
