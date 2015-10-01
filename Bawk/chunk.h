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

#include "glm/glm.hpp"

// dimensions of our chunk
#define CX 16
#define CY 16
#define CZ 16

typedef glm::tvec4<GLbyte> byte4;

class Chunk {
    uint8_t blk[CX][CY][CZ];    // blk type for each block
    GLuint vbo;                 // reference to VBO (vertex buffer object)
    int elements;               // number of blocks that need rendering
    bool changed;               // if chunk is updated, set this
public:
    Chunk();
    void remove();
    uint8_t get(int x, int y, int z);
    void set(int x, int y, int z, uint8_t type);
    void update();
    void render();
};

#endif
