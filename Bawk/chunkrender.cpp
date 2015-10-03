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
#include "block.h"

// dimensions of a chunk
#define CX 16
#define CY 16
#define CZ 16

// number of chunks to have loaded at any one time
#define CHUNKSLOTS 3000

// slots for VBO for the chunks. This gets cycled if we don't have enough
static struct RenderableChunk *chunk_slot[CHUNKSLOTS] = {0};

void set_coord_and_texture(GLbyte coord[][3],
                           GLbyte texture[][3],
                           int index,
                           int x, int y, int z,
                           uint16_t type, uint8_t flags) {
    coord[index][0] = x;
    coord[index][1] = y;
    coord[index][2] = z;
    
    // first 8 bits. Represents the x-axis in our texture atlas
    texture[index][0] = type;
    // last 8 bits. Represents the y-axis in our texture atlas
    texture[index][1] = type >> 0x8;
    // some extra flags we can set to let shader know how to render this
    texture[index][2] = flags;
}

RenderableChunk::RenderableChunk() {
    memset(blk, 0, sizeof blk);
    elements = 0;
    slot = 0;
    lastused = glfwGetTime();
    changed = true;
    
    left = right = below = above = front = back = 0;
    
    for (int x = 0; x < CX; x++) {
        for (int y = 0; y < CY; y++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 10 < 3) {
                    //blk[x][y][z] = (uint16_t)(rand() * sizeof(uint16_t));
                    blk[x][y][z] = rand() % 3;
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

bool RenderableChunk::isblocked(int x1, int y1, int z1, int x2, int y2, int z2) {
    // Air (0) is always "blocked"
    if(!blk[x1][y1][z1])
        return true;
    
    // Leaves do not block any other block, including themselves
    if(get_transparency(get(x2, y2, z2)) == 1)
        return false;
    
    // Non-transparent blocks always block line of sight
    if(!get_transparency(get(x2, y2, z2)))
        return true;
    
    // Otherwise, LOS is only blocked by blocks if the same transparency type
    return get_transparency(get(x2, y2, z2)) == get_transparency(blk[x1][y1][z1]);
}


uint16_t RenderableChunk::get(int x, int y, int z) {
    if(x < 0)
        return left ? left->blk[x + CX][y][z] : 0;
    if(x >= CX)
        return right ? right->blk[x - CX][y][z] : 0;
    if(y < 0)
        return below ? below->blk[x][y + CY][z] : 0;
    if(y >= CY)
        return above ? above->blk[x][y - CY][z] : 0;
    if(z < 0)
        return front ? front->blk[x][y][z + CZ] : 0;
    if(z >= CZ)
        return back ? back->blk[x][y][z - CZ] : 0;
    
    return blk[x][y][z];
}

void RenderableChunk::set(int x, int y, int z, uint16_t type) {
    blk[x][y][z] = type;
    changed = true;
}


void RenderableChunk::update() {
    changed = false;
    
    // 6 faces per cube, plus 3+3 vertices on each triangle of quad
    // but half will not be visible for some reason...don't render?
    GLbyte vertex[CX * CY * CZ * 18][3];
    // I use 2 bytes to denote block type, then 1 byte for flags
    GLbyte texture[CX * CY * CZ * 18][3];
    int i = 0;
    int merged = 0;
    bool vis = false;
    
    // View from negative x
    
    for(int x = CX - 1; x >= 0; x--) {
        for(int y = 0; y < CY; y++) {
            for(int z = 0; z < CZ; z++) {
                // Line of sight blocked?
                if(isblocked(x, y, z, x - 1, y, z)) {
                    vis = false;
                    continue;
                }
                uint16_t type = blk[x][y][z];
                uint8_t flags = 0;
                
                // Same block as previous one? Extend it.
                if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
                    set_coord_and_texture(vertex, texture, i - 5, x, y, z + 1, type, flags);
                    
                    set_coord_and_texture(vertex, texture, i - 2, x, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 1, x, y + 1, z + 1, type, flags);
                    merged++;
                    // Otherwise, add a new quad.
                } else {
                    set_coord_and_texture(vertex, texture, i++, x, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z + 1, type, flags);
                }
                
                vis = true;
            }
        }
    }
    
    // View from positive x
    
    for(int x = 0; x < CX; x++) {
        for(int y = 0; y < CY; y++) {
            for(int z = 0; z < CZ; z++) {
                if(isblocked(x, y, z, x + 1, y, z)) {
                    vis = false;
                    continue;
                }
                uint16_t type = blk[x][y][z];
                uint8_t flags = 0;
                
                if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
                    set_coord_and_texture(vertex, texture, i - 4, x + 1, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 2, x + 1, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 1, x + 1, y, z + 1, type, flags);
                    merged++;
                } else {
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z + 1, type, flags);
                }
                vis = true;
            }
        }
    }
    
    // View from negative y
    
    for(int x = 0; x < CX; x++) {
        for(int y = CY - 1; y >= 0; y--) {
            for(int z = 0; z < CZ; z++) {
                if(isblocked(x, y, z, x, y - 1, z)) {
                    vis = false;
                    continue;
                }
                uint16_t type = blk[x][y][z];
                uint8_t flags = 0x1;
                
                if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
                    set_coord_and_texture(vertex, texture, i - 4, x, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 2, x + 1, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 1, x, y, z + 1, type, flags);
                    merged++;
                } else {
                    set_coord_and_texture(vertex, texture, i++, x, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y, z + 1, type, flags);
                }
                vis = true;
            }
        }
    }
    
    // View from positive y
    
    for(int x = 0; x < CX; x++) {
        for(int y = 0; y < CY; y++) {
            for(int z = 0; z < CZ; z++) {
                if(isblocked(x, y, z, x, y + 1, z)) {
                    vis = false;
                    continue;
                }
                uint16_t type = blk[x][y][z];
                uint8_t flags = 0x1;
                
                if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
                    set_coord_and_texture(vertex, texture, i - 5, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 2, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 1, x + 1, y + 1, z + 1, type, flags);
                    merged++;
                } else {
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z + 1, type, flags);
                }
                vis = true;
            }
        }
    }
    
    // View from negative z
    
    for(int x = 0; x < CX; x++) {
        for(int z = CZ - 1; z >= 0; z--) {
            for(int y = 0; y < CY; y++) {
                if(isblocked(x, y, z, x, y, z - 1)) {
                    vis = false;
                    continue;
                }
                uint16_t type = blk[x][y][z];
                uint8_t flags = 0;
                
                if(vis && y != 0 && blk[x][y][z] == blk[x][y - 1][z]) {
                    set_coord_and_texture(vertex, texture, i - 5, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i - 3, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i - 2, x + 1, y + 1, z, type, flags);
                    merged++;
                } else {
                    set_coord_and_texture(vertex, texture, i++, x, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z, type, flags);
                }
                vis = true;
            }
        }
    }
    
    // View from positive z
    
    for(int x = 0; x < CX; x++) {
        for(int z = 0; z < CZ; z++) {
            for(int y = 0; y < CY; y++) {
                if(isblocked(x, y, z, x, y, z + 1)) {
                    vis = false;
                    continue;
                }
                uint16_t type = blk[x][y][z];
                uint8_t flags = 0;
                
                if(vis && y != 0 && blk[x][y][z] == blk[x][y - 1][z]) {
                    set_coord_and_texture(vertex, texture, i - 4, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 3, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i - 1, x + 1, y + 1, z + 1, type, flags);
                    merged++;
                } else {
                    set_coord_and_texture(vertex, texture, i++, x, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x, y + 1, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y, z + 1, type, flags);
                    set_coord_and_texture(vertex, texture, i++, x + 1, y + 1, z + 1, type, flags);
                }
                vis = true;
            }
        }
    }
    
    changed = false;
    elements = i;
    
    printf("Finished generating %d elements\n", elements);
    
    // If this chunk is empty, no need to allocate a chunk slot.
    if(!elements)
        return;
    
    // If we don't have an active slot, find one
    if(chunk_slot[slot] != this) {
        int lru = 0;
        for(int i = 0; i < CHUNKSLOTS; i++) {
            // If there is an empty slot, use it
            if(!chunk_slot[i]) {
                lru = i;
                break;
            }
            // Otherwise try to find the least recently used slot
            if(chunk_slot[i]->lastused < chunk_slot[lru]->lastused)
                lru = i;
        }
        
        // If the slot is empty, create a new VBO
        if(!chunk_slot[lru]) {
            glGenBuffers(1, &coord_vbo);
            glGenBuffers(1, &texture_vbo);
            // Otherwise, steal it from the previous slot owner
        } else {
            coord_vbo = chunk_slot[lru]->coord_vbo;
            texture_vbo = chunk_slot[lru]->texture_vbo;
            chunk_slot[lru]->changed = true;
        }
        
        slot = lru;
        chunk_slot[slot] = this;
    }
    
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
