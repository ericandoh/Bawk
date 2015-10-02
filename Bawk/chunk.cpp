//
//  chunk.cpp
//  Bawk
//
//  Created by Eric Oh on 9/30/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <stdio.h>
#include <cstring>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "worldrender.h"

#define CX 16
#define CY 16
#define CZ 16

typedef glm::tvec4<GLbyte> byte4;

Chunk::Chunk() {
    memset(blk, 0, sizeof blk);
    elements = 0;
    changed = true;
    
    for (int x = 0; x < CX; x++) {
        for (int y = 0; y < CY; y++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 10 < 3) {
                    blk[x][y][z] = (uint8_t)(rand() * sizeof(uint8_t));
                }
            }
        }
    }
    
    glGenBuffers(1, &vbo);
}

void Chunk::remove() {
    glDeleteBuffers(1, &vbo);
}

uint8_t Chunk::get(int x, int y, int z) {
    return blk[x][y][z];
}

void Chunk::set(int x, int y, int z, uint8_t type) {
    blk[x][y][z] = type;
    changed = true;
}

void Chunk::update() {
    changed = false;
    
    // 6 faces per cube, plus 3+3 vertices on each triangle of quad
    byte4 vertex[CX * CY * CZ * 6 * 6];
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
                vertex[i++] = byte4(x,     y,     z,     type);
                vertex[i++] = byte4(x,     y,     z + 1, type);
                vertex[i++] = byte4(x,     y + 1, z,     type);
                vertex[i++] = byte4(x,     y + 1, z,     type);
                vertex[i++] = byte4(x,     y,     z + 1, type);
                vertex[i++] = byte4(x,     y + 1, z + 1, type);
                
                // View from positive x
                vertex[i++] = byte4(x + 1, y,     z,     type);
                vertex[i++] = byte4(x + 1, y + 1, z,     type);
                vertex[i++] = byte4(x + 1, y,     z + 1, type);
                vertex[i++] = byte4(x + 1, y + 1, z,     type);
                vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
                vertex[i++] = byte4(x + 1, y    , z + 1, type);
                
                // View from negative y
                vertex[i++] = byte4(x,     y,     z,     type);
                vertex[i++] = byte4(x + 1, y,     z,     type);
                vertex[i++] = byte4(x,     y,     z + 1, type);
                vertex[i++] = byte4(x,     y,     z + 1, type);
                vertex[i++] = byte4(x + 1, y,     z,     type);
                vertex[i++] = byte4(x + 1, y,     z + 1, type);
                
                // View from positive y
                vertex[i++] = byte4(x,     y + 1, z,     type);
                vertex[i++] = byte4(x,     y + 1, z + 1, type);
                vertex[i++] = byte4(x + 1, y + 1, z    , type);
                vertex[i++] = byte4(x + 1, y + 1, z,     type);
                vertex[i++] = byte4(x    , y + 1, z + 1, type);
                vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
                
                // View from negative z
                vertex[i++] = byte4(x,     y,     z,     type);
                vertex[i++] = byte4(x,     y + 1, z,     type);
                vertex[i++] = byte4(x + 1, y,     z,     type);
                vertex[i++] = byte4(x + 1, y,     z,     type);
                vertex[i++] = byte4(x,     y + 1, z,     type);
                vertex[i++] = byte4(x + 1, y + 1, z,     type);
                
                // View from positive z
                vertex[i++] = byte4(x,     y,     z + 1,  type);
                vertex[i++] = byte4(x + 1, y,     z + 1, type);
                vertex[i++] = byte4(x,     y + 1, z + 1, type);
                vertex[i++] = byte4(x,     y + 1, z + 1, type);
                vertex[i++] = byte4(x + 1, y,     z + 1, type);
                vertex[i++] = byte4(x + 1, y + 1, z + 1, type);
            }
        }
    }
    
    elements = i;
    printf("Finished generating %d elements\n", elements);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);
}

void Chunk::render() {
    if(changed)
        update();
    // If this chunk is empty, we don't need to draw anything.
    if(!elements)
        return;
    
    // lets us know to use this program shaders
    glUseProgram(program);
    
    // Render the VBO here
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(block_attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, elements);
}
