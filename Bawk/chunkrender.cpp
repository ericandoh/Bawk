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

void set_coord_and_texture(GLbyte coord[][3],
                           GLbyte texture[][3],
                           int index,
                           int x, int y, int z,
                           uint16_t type, uint8_t flags) {
    coord[index][0] = x;
    coord[index][1] = y;
    coord[index][2] = z;
    
    // first 8 bits. Represents the x-axis in our texture atlas
    texture[index][0] = type >> 0x8;
    // last 8 bits. Represents the y-axis in our texture atlas
    texture[index][1] = type;
    // some extra flags we can set to let shader know how to render this
    texture[index][2] = flags;
}

RenderableChunk::RenderableChunk() {
    memset(blk, 0, sizeof blk);
    elements = 0;
    changed = true;
    
    for (int x = 0; x < CX; x++) {
        for (int y = 0; y < CY; y++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 10 < 3) {
                    blk[x][y][z] = (uint16_t)(rand() * sizeof(uint16_t));
                }
            }
        }
    }
    glGenBuffers(1, &coord_vbo);
    glGenBuffers(1, &texture_vbo);
}

RenderableChunk::~RenderableChunk() {
    glDeleteBuffers(1, &coord_vbo);
    glDeleteBuffers(1, &texture_vbo);
}

void RenderableChunk::update() {
    changed = false;
    
    // 6 faces per cube, plus 3+3 vertices on each triangle of quad
    GLbyte vertex[CX * CY * CZ * 6 * 6][3];
    // I use 2 bytes to denote block type, then 1 byte for flags
    GLbyte texture[CX * CY * CZ * 6 * 6][3];
    int i = 0;
    
    for(int x = 0; x < CX; x++) {
        for(int y = 0; y < CY; y++) {
            for(int z = 0; z < CZ; z++) {
                uint16_t type = blk[x][y][z];
                uint8_t flags = 123;
                
                // Empty block?
                if(!type)
                    continue;
                
                // TODO can optimize by not rendering faces that are blocked
                
                // View from negative x
                set_coord_and_texture(vertex, texture, i++, x,     y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z + 1, type, flags);
                
                // View from positive x
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y    , z + 1, type, flags);
                
                // View from negative y
                set_coord_and_texture(vertex, texture, i++, x,     y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z + 1, type, flags);
                
                // View from positive y
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z    , type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x    , y + 1, z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z + 1, type, flags);
                
                // View from negative z
                set_coord_and_texture(vertex, texture, i++, x,     y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z,     type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z,     type, flags);
                
                // View from positive z
                set_coord_and_texture(vertex, texture, i++, x,     y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x,     y + 1, z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y,     z + 1, type, flags);
                set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z + 1, type, flags);
            }
        }
    }
    
    elements = i;
    printf("Finished generating %d elements\n", elements);
    
    glBindBuffer(GL_ARRAY_BUFFER, coord_vbo);
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof *texture, texture, GL_STATIC_DRAW);
}

void RenderableChunk::render() {
    if(changed)
        update();
    // If this chunk is empty, we don't need to draw anything.
    if(!elements)
        return;
    
    // lets us know to use this program shaders
    glUseProgram(program);
    
    // Render the VBO here
    glBindBuffer(GL_ARRAY_BUFFER, coord_vbo);
    glVertexAttribPointer(block_attribute_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
    glVertexAttribPointer(texture_attribute_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, elements);
}
