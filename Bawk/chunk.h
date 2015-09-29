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
#include <cstring>
#include "display.h"

// dimensions of our chunk
#define CX 16
#define CY 16
#define CZ 16

// a struct describing the position and type of a block
struct BlockInfo {
    uint8_t x, y, z;
    uint8_t type;   // change this to int later to support more varied types
    BlockInfo(uint8_t ix=0, uint8_t iy=0, uint8_t iz=0, uint8_t itype=0):
        x(ix), y(iy), z(iz), type(itype){}
};

struct chunk {
    uint8_t blk[CX][CY][CZ];    // blk type for each block
    GLuint vbo;                 // reference to VBO (vertex buffer object)
    int elements;               // number of blocks that need rendering
    bool changed;               // if chunk is updated, set this
    
    chunk() {
        memset(blk, 0, sizeof blk);
        elements = 0;
        changed = true;
        glGenBuffers(1, &vbo);
    }
    
    ~chunk() {
        glDeleteBuffers(1, &vbo);
    }
    
    uint8_t get(int x, int y, int z) {
        return blk[x][y][z];
    }
    
    void set(int x, int y, int z, uint8_t type) {
        blk[x][y][z] = type;
        changed = true;
    }
    
    void update() {
        changed = false;
        
        // 6 faces per cube, plus 3+3 vertices on each triangle of quad
        BlockInfo vertex[CX * CY * CZ * 6 * 6];
        int i = 0;
        
        for(int x = 0; x < CX; x++) {
            for(int y = 0; y < CY; y++) {
                for(int z = 0; z < CZ; z++) {
                    uint8_t type = blk[x][y][z];
                    
                    // Empty block?
                    if(!type)
                        continue;
                    
                    // TODO can optimize by not rendering faces that are blocked
                    
                    // View from negative x
                    vertex[i++] = BlockInfo(x,     y,     z,     type);
                    vertex[i++] = BlockInfo(x,     y,     z + 1, type);
                    vertex[i++] = BlockInfo(x,     y + 1, z,     type);
                    vertex[i++] = BlockInfo(x,     y + 1, z,     type);
                    vertex[i++] = BlockInfo(x,     y,     z + 1, type);
                    vertex[i++] = BlockInfo(x,     y + 1, z + 1, type);
                    
                    // View from positive x
                    vertex[i++] = BlockInfo(x + 1, y,     z,     type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z,     type);
                    vertex[i++] = BlockInfo(x + 1, y,     z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y    , z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z,     type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z + 1, type);
                    
                    // Repeat for +y, -y, +z, and -z directions
                    
                    // View from negative y
                    vertex[i++] = BlockInfo(x,     y,     z,     type);
                    vertex[i++] = BlockInfo(x + 1, y,     z,     type);
                    vertex[i++] = BlockInfo(x,     y,     z + 1, type);
                    vertex[i++] = BlockInfo(x,     y,     z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y,     z,     type);
                    vertex[i++] = BlockInfo(x + 1, y,     z + 1, type);
                    
                    // View from positive y
                    vertex[i++] = BlockInfo(x,     y + 1, z,     type);
                    vertex[i++] = BlockInfo(x,     y + 1, z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z    , type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z,     type);
                    vertex[i++] = BlockInfo(x    , y + 1, z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z + 1, type);
                    
                    // View from negative z
                    vertex[i++] = BlockInfo(x,     y,     z,     type);
                    vertex[i++] = BlockInfo(x,     y + 1, z,     type);
                    vertex[i++] = BlockInfo(x + 1, y,     z,     type);
                    vertex[i++] = BlockInfo(x + 1, y,     z,     type);
                    vertex[i++] = BlockInfo(x,     y + 1, z,     type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z,     type);
                    
                    // View from positive z
                    vertex[i++] = BlockInfo(x,     y,     z + 1,  type);
                    vertex[i++] = BlockInfo(x + 1, y,     z + 1, type);
                    vertex[i++] = BlockInfo(x,     y + 1, z + 1, type);
                    vertex[i++] = BlockInfo(x,     y + 1, z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y,     z + 1, type);
                    vertex[i++] = BlockInfo(x + 1, y + 1, z + 1, type);
                    
                }
            }
        }
        
        elements = i;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, elements * sizeof(BlockInfo), &vertex[0].x, GL_STATIC_DRAW);
    }
    
    void render() {
        if(changed)
            update();
        // Render the VBO here
        // If this chunk is empty, we don't need to draw anything.
        if(!elements)
            return;
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // going into attribute index 0, holding 4 bytes? per vertex
        glVertexAttribPointer(block_attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, elements);
    }
};

#endif
