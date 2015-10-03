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
    int slot;                   // which slot it is using for the VBO?
    double lastused;
    RenderableChunk *left, *right, *below, *above, *front, *back;
    bool isblocked(int x1, int y1, int z1, int x2, int y2, int z2);
protected:
    uint16_t blk[CX][CY][CZ];   // blk type for each block
    bool changed;               // if chunk is updated, set this
public:
    RenderableChunk();
    ~RenderableChunk();
    virtual int get_transparency(uint16_t block) = 0;
    uint16_t get(int x, int y, int z);
    void set(int x, int y, int z, uint16_t type);
    void update();
    void render();
};

#endif
