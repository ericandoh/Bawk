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

void delete_all_buffers();

class RenderableChunk {
    GLuint coord_vbo;           // VBO (vertex buffer object) for coordinates
    GLuint texture_vbo;         // VBO (vertex buffer object) for textures/flags
    int elements;               // number of triangles that need rendering
    int block_counter;          // number of blocks in this chunk
    int slot;                   // which slot it is using for the VBO?
    double lastused;
    // internal function used to optimize rendering of blocks
    bool isblocked(int x1, int y1, int z1, int x2, int y2, int z2);
public:
    // these set public for ease of setting
    RenderableChunk *left, *right, *below, *above, *front, *back;
    // blk type for each block
    uint16_t blk[CX][CY][CZ];
    // bounding box around chunk contents that isn't 0 (isn't air)
    ivec3 lower_bound, upper_bound;
    // if chunk is updated, set this
    bool changed;
    ~RenderableChunk();
    RenderableChunk(uint16_t from[CX][CY][CZ]);
    // get block at x, y, z in chunk's coordinates
    uint16_t get(int x, int y, int z);
    // set block at x, y, z in chunk's coordinates
    void set(int x, int y, int z, uint16_t type);
    // get min(x,y,z) and max(x,y,z) over all blocks in this chunk (in chunk's coordinates)
    void update_dimensions();
    // sees if a bounding box (in CRC) intersects the
    bool intersects_my_bounds(ivec3 lower_corner, ivec3 upper_corner);
    // remakes and rebinds block vertex data to VBO
    void update();
    // renders the chunk, given that the transformation matrix has been set previously correctly
    void render();
};

// fills a CX by CY by CZ array with empty data representing empty (air) blocks
void get_empty_chunk(uint16_t source[CX][CY][CZ]);

#endif
