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

#include "importopengl.h"
#include <vector>
#include "block.h"
#include "lightrender.h"
#include "settings.h"

void delete_all_buffers();

struct LightAndPosition {
    RenderableLight* light;
    fvec3 pos;
};

class RenderableChunk {
    GLuint coord_vbo;           // VBO (vertex buffer object) for coordinates
    GLuint texture_vbo;         // VBO (vertex buffer object) for textures/flags
    std::vector<LightAndPosition> lights;
    // TODO DEPRECATE THIS into our VBO function
    std::vector<fvec3> model_vertices;
    std::vector<fvec3> model_normals;
    std::vector<fvec3> model_uvs;
    int elements;               // number of triangles that need rendering
    int block_counter;          // number of blocks in this chunk
    int slot;                   // which slot it is using for the VBO?
    int resource_slot;
    uint32_t lastused;
    // internal function used to optimize rendering of blocks
    bool isblocked(int x1, int y1, int z1, int x2, int y2, int z2);
public:
    bool has_lights;
    // these set public for ease of setting
    RenderableChunk *left, *right, *below, *above, *front, *back;
    // blk type for each block
    block_type blk[CX][CY][CZ];
    // bounding box around chunk contents that isn't 0 (isn't air)
    ivec3 lower_bound, upper_bound;
    // if chunk is updated, set this
    bool changed;
    RenderableChunk();
    RenderableChunk(block_type from[CX][CY][CZ]);
    void cleanup();
    void reset();
    // get block at x, y, z in chunk's coordinates
    block_type* get(int x, int y, int z);
    // set block at x, y, z in chunk's coordinates
    void set(int x, int y, int z, block_type type);
    // get min(x,y,z) and max(x,y,z) over all blocks in this chunk (in chunk's coordinates)
    void update_dimensions();
    // sees if a bounding box (in CRC) intersects the
    bool intersects_my_bounds(ivec3 lower_corner, ivec3 upper_corner);
    // remakes and rebinds block vertex data to VBO
    void update();
    // renders the chunk, given that the transformation matrix has been set previously correctly
    void render();
    void render_lights(fmat4* transform, fvec3 player_pos);
};

// fills a CX by CY by CZ array with empty data representing empty (air) blocks
void get_empty_chunk(block_type source[CX][CY][CZ]);

#endif
