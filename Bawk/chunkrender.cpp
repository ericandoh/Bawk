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
#include "blockrender.h"
#include "game_info_loader.h"

// dimensions of a chunk
#define CX 16
#define CY 16
#define CZ 16

// number of chunks to have loaded at any one time
#define CHUNKSLOTS 3000

struct vboholder {
    RenderableChunk* owner;
    GLuint coord_vbo;
    GLuint texture_vbo;
};

// slots for VBO for the chunks. This gets cycled if we don't have enough
static vboholder chunk_slot[CHUNKSLOTS] = {0};

RenderableChunk::RenderableChunk(block_type from[CX][CY][CZ]) {
    memset(blk, 0, sizeof blk);
    elements = 0;
    block_counter = -1;
    slot = 0;
    lastused = glfwGetTime();
    changed = true;
    left = right = below = above = front = back = 0;
    memcpy(&blk[0][0][0], &from[0][0][0], sizeof(block_type)*CX*CY*CZ);
    update_dimensions();
}

void RenderableChunk::cleanup() {
    // removing this chunk
    // free its slot
    if(chunk_slot[slot].owner == this) {
        chunk_slot[slot].owner = 0;
    }
    
    // detach from all its chunky neighbors if they exist
    if (left) {
        left->right = 0;
        left->changed = true;
    }
    if (right) {
        right->left = 0;
        right->changed = true;
    }
    if (below) {
        below->above = 0;
        below->changed = true;
    }
    if (above) {
        above->below = 0;
        above->changed = true;
    }
    if (front) {
        front->back = 0;
        front->changed = true;
    }
    if (back) {
        back->front = 0;
        back->changed = true;
    }
}

void delete_all_buffers() {
    for(int i = 0; i < CHUNKSLOTS; i++) {
        // List is sequential, so first empty cell is end of list
        if(!chunk_slot[i].owner) {
            break;
        }
        // Delete buffers that remain
        glDeleteBuffers(1, &chunk_slot[i].coord_vbo);
        glDeleteBuffers(1, &chunk_slot[i].texture_vbo);
    }
}

bool RenderableChunk::isblocked(int x1, int y1, int z1, int x2, int y2, int z2) {
    // Air (0) is always "blocked"
    if(!blk[x1][y1][z1].type)
        return true;
    
    // Recipe Models are also always blocked, because they dont need to be rendered
    if (get_block_is_model(blk[x1][y1][z1]))
        return true;
    
    // Things next to models however, are rendered (models are sexy!)
    if (get_block_is_model(get(x2, y2, z2)))
        return false;
    
    // Leaves do not block any other block, including themselves
    if(get_transparency(get(x2, y2, z2).type) == 1)
        return false;
    
    // Non-transparent blocks always block line of sight
    if(!get_transparency(get(x2, y2, z2).type))
        return true;
    
    // Otherwise, LOS is only blocked by blocks if the same transparency type
    return get_transparency(get(x2, y2, z2).type) == get_transparency(blk[x1][y1][z1].type);
}


block_type RenderableChunk::get(int x, int y, int z) {
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

void RenderableChunk::set(int x, int y, int z, block_type type) {
    uint16_t prev = blk[x][y][z].type;
    blk[x][y][z] = type;
    if (!prev) {
        // assume i'm adding a block
        block_counter++;
    }
    if (!type.type) {
        // pysch i'm actually not adding a block
        block_counter--;
        if (prev) {
            // i removed a block. dude...
            bool anyMatchingLower = (x == lower_bound.x) || (y == lower_bound.y) || (z == lower_bound.z);
            bool anyMatchingUpper = (x == upper_bound.x) || (y == upper_bound.y) || (z == upper_bound.z);
            if (anyMatchingLower || anyMatchingUpper) {
                // we removed a block on the periphery. update dimensions!
                update_dimensions();
            }
        }
    }
    else {
        // update lower_bound, upper_bound
        
        lower_bound.x = imin(x, lower_bound.x);
        lower_bound.y = imin(y, lower_bound.y);
        lower_bound.z = imin(z, lower_bound.z);
        upper_bound.x = imax(x, upper_bound.x);
        upper_bound.y = imax(y, upper_bound.y);
        upper_bound.z = imax(z, upper_bound.z);
    }
    // When updating blocks at the edge of this chunk,
    // visibility of blocks in the neighbouring chunk might change.
    if(x == 0 && left)
        left->changed = true;
    if(x == CX - 1 && right)
        right->changed = true;
    if(y == 0 && below)
        below->changed = true;
    if(y == CY - 1 && above)
        above->changed = true;
    if(z == 0 && front)
        front->changed = true;
    if(z == CZ - 1 && back)
        back->changed = true;
    changed = true;
}

void RenderableChunk::update_dimensions() {
    // TOFU could this be slightly more efficient? a loop over 4k elements isnt the most efficient...
    // small, large: inclusive-inclusive
    lower_bound = ivec3(CX, CY, CZ);
    upper_bound = ivec3(0, 0, 0);
    if (block_counter == 0)
        return;
    block_counter = 0;
    for (int x = 0; x < CX; x++) {
        for (int y = 0; y < CY; y++) {
            for (int z = 0; z < CZ; z++) {
                if (blk[x][y][z].type) {
                    block_counter++;
                    lower_bound.x = imin(x, lower_bound.x);
                    lower_bound.y = imin(y, lower_bound.y);
                    lower_bound.z = imin(z, lower_bound.z);
                    upper_bound.x = imax(x, upper_bound.x);
                    upper_bound.y = imax(y, upper_bound.y);
                    upper_bound.z = imax(z, upper_bound.z);
                }
            }
        }
    }
}

bool RenderableChunk::intersects_my_bounds(ivec3 lower_corner, ivec3 upper_corner) {
    return   !(lower_bound.x > upper_corner.x || lower_corner.x > upper_bound.x
            || lower_bound.y > upper_corner.y || lower_corner.y > upper_bound.y
            || lower_bound.z > upper_corner.z || lower_corner.z > upper_bound.z);
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
    
    model_vertices.clear();
    model_normals.clear();
    model_uvs.clear();
    
    // View from negative x
    
    for(int x = CX - 1; x >= 0; x--) {
        for(int y = 0; y < CY; y++) {
            for(int z = 0; z < CZ; z++) {
                if (get_block_is_model(blk[x][y][z])) {
                    // add to
                    fill_game_models(model_vertices, model_normals, model_uvs, blk[x][y][z], x, y, z);
                }
                // Line of sight blocked?
                if(isblocked(x, y, z, x - 1, y, z)) {
                    vis = false;
                    continue;
                }
                block_type type = blk[x][y][z];
                BlockOrientation flags = BlockOrientation::BACK;
                
                // Same block as previous one? Extend it.
                if(vis && z != 0 && blk[x][y][z].equals(blk[x][y][z - 1])) {
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
                block_type type = blk[x][y][z];
                BlockOrientation flags = BlockOrientation::FRONT;
                
                if(vis && z != 0 && blk[x][y][z].equals(blk[x][y][z - 1])) {
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
                block_type type = blk[x][y][z];
                BlockOrientation flags = BlockOrientation::BOTTOM;
                
                if(vis && z != 0 && blk[x][y][z].equals(blk[x][y][z - 1])) {
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
                block_type type = blk[x][y][z];
                BlockOrientation flags = BlockOrientation::TOP;
                
                if(vis && z != 0 && blk[x][y][z].equals(blk[x][y][z - 1])) {
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
                block_type type = blk[x][y][z];
                BlockOrientation flags = BlockOrientation::LEFT;
                
                if(vis && y != 0 && blk[x][y][z].equals(blk[x][y - 1][z])) {
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
                block_type type = blk[x][y][z];
                BlockOrientation flags = BlockOrientation::RIGHT;
                
                if(vis && y != 0 && blk[x][y][z].equals(blk[x][y - 1][z])) {
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
    
    //printf("Finished generating %d elements\n", elements);
    
    // If this chunk is empty, no need to allocate a chunk slot.
    if(!elements)
        return;
    
    // If we don't have an active slot, find one
    if(chunk_slot[slot].owner != this) {
        int lru = 0;
        for(int i = 0; i < CHUNKSLOTS; i++) {
            // If there is an empty slot, use it
            if(!chunk_slot[i].owner) {
                lru = i;
                break;
            }
            // Otherwise try to find the least recently used slot
            if(chunk_slot[i].owner->lastused < chunk_slot[lru].owner->lastused)
                lru = i;
        }
        
        // If the slot is empty, create a new VBO
        if(!chunk_slot[lru].owner) {
            if (!(chunk_slot[lru].coord_vbo) || !(chunk_slot[lru].texture_vbo)) {
                glGenBuffers(1, &chunk_slot[lru].texture_vbo);
                glGenBuffers(1, &chunk_slot[lru].coord_vbo);
            }
        } else {
            // Otherwise, steal it from the previous slot owner
            chunk_slot[lru].owner->changed = true;
        }
        coord_vbo = chunk_slot[lru].coord_vbo;
        texture_vbo = chunk_slot[lru].texture_vbo;
        
        slot = lru;
        chunk_slot[slot].owner = this;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, coord_vbo);
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, elements * sizeof *texture, texture, GL_STATIC_DRAW);
}

void RenderableChunk::render() {
    if(changed)
        update();
    
    lastused = glfwGetTime();
    
    // If this chunk is empty, we don't need to draw anything.
    if(elements) {
        // we're drawing blocks
        set_block_draw_mode(1);
        
        // Render the VBO here
        glBindBuffer(GL_ARRAY_BUFFER, coord_vbo);
        glVertexAttribPointer(block_attribute_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
        glVertexAttribPointer(texture_attribute_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, elements);
    }
    
    if (model_vertices.size() > 0) {
        // draw the model elements
        set_block_draw_mode(0);
        int num_triangles = (int)model_vertices.size();
        
        glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
        glBufferData(GL_ARRAY_BUFFER, model_vertices.size() * sizeof(fvec3), &(model_vertices[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
        glBufferData(GL_ARRAY_BUFFER, model_uvs.size() * sizeof(fvec3), &(model_uvs[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(texture_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, num_triangles);
    }
}

void get_empty_chunk(block_type source[CX][CY][CZ]) {
    memset(source, 0, sizeof(source[0][0][0])*CX*CY*CZ);
}