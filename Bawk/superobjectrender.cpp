//
//  superobjectrender.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>
#include "worldrender.h"
#include "superobjectrender.h"


RenderableSuperObject::~RenderableSuperObject() {
    for (auto kv : chunks) {
        delete kv.second;
    }
}

int RenderableSuperObject::load_chunk(int x, int y, int z) {
    printf("Loading chunk for %d %d %d\n", x, y, z);
    uint16_t raw_chunk[CX][CY][CZ];
    if (get_chunk(raw_chunk, x, y, z)) {
        // failed to load chunk
        return 1;
    }
    RenderableChunk* chunk = new RenderableChunk(raw_chunk);
    chunks[ivec3(x, y, z)] = chunk;
    
    ivec3 left(x - 1, y, z);
    ivec3 right(x + 1, y, z);
    ivec3 below(x, y - 1, z);
    ivec3 above(x, y + 1, z);
    ivec3 front(x, y, z - 1);
    ivec3 back(x, y, z);

    if (chunks.count(left)) {
        chunk->left = chunks[left];
        chunk->left->changed = true;
    }
    if (chunks.count(right)) {
        chunk->right = chunks[right];
        chunk->right->changed = true;
    }
    if (chunks.count(below)) {
        chunk->below = chunks[below];
        chunk->below->changed = true;
    }
    if (chunks.count(above)) {
        chunk->above = chunks[above];
        chunk->above->changed = true;
    }
    if (chunks.count(front)) {
        chunk->front = chunks[front];
        chunk->front->changed = true;
    }
    if (chunks.count(back)) {
        chunk->back = chunks[back];
        chunk->back->changed = true;
    }
    
    return 0;
}

void RenderableSuperObject::delete_chunk(int x, int y, int z) {
    ivec3 pos(x, y, z);
    if (!chunks.count(pos)) {
        // this chunk is already gone mate
        return;
    }
    // this will call destructor which will (hopefully) cleanly erase/detach the chunk!
    chunks.erase(pos);
}

uint16_t RenderableSuperObject::get_block(int x, int y, int z) {
    int x_offset = (x % CX + CX) % CX;
    int y_offset = (y % CY + CY) % CY;
    int z_offset = (z % CZ + CZ) % CZ;
    ivec3 pos = ivec3((x - x_offset) / CX, (y - y_offset) / CY, (z - z_offset) / CZ);
    
    if (chunks.count(pos)) {
        // the superobject has the object within its range
        return chunks[pos]->get(x_offset, y_offset, z_offset);
    }
    
    // the chunk was not found, so try loading the chunk and try again
    if (!load_chunk(pos.x, pos.y, pos.z)) {
        return chunks[pos]->get(x_offset, y_offset, z_offset);
    }
    // loading the chunk failed...return air
    return 0;
}

void RenderableSuperObject::set_block(int x, int y, int z, uint16_t type) {
    int x_offset = (x % CX + CX) % CX;
    int y_offset = (y % CY + CY) % CY;
    int z_offset = (z % CZ + CZ) % CZ;
    ivec3 pos = ivec3((x - x_offset) / CX, (y - y_offset) / CY, (z - z_offset) / CZ);
    
    if (chunks.count(pos)) {
        // the superobject has the object within its range
        chunks[pos]->set(x_offset, y_offset, z_offset, type);
        return;
    }
    // the chunk was not found, so try loading the chunk and try again
    if (!load_chunk(pos.x, pos.y, pos.z)) {
        chunks[pos]->set(x_offset, y_offset, z_offset, type);
    }
    // loading the chunk failed, probably because this is out of bounds
}

void RenderableSuperObject::render(fmat4* transform) {
    for (auto iterator = chunks.begin(); iterator != chunks.end(); iterator++) {
        ivec3 pos = iterator->first;
        
        fmat4 model = glm::translate(fmat4(1.0f),
                                     fvec3(pos.x * CX,
                                           pos.y * CY,
                                           pos.z * CZ));
        fmat4 mvp = *transform * model;
        
        // Is this chunk on the screen?
        glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
        
        //float d = glm::length(center);
        center.x /= center.w;
        center.y /= center.w;
        
        // If it is behind the camera, don't bother drawing it
        if(center.z < -CY / 2)
            continue;
        
        // If it is outside the screen, don't bother drawing it
        if(fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
            continue;
        
        
        set_transform_matrix(mvp);
        iterator->second->render();
    }
}

int minimum(int a, int b) {
    return a < b ? a : b;
}

int maximum(int a, int b) {
    return a > b ? a : b;
}

bool is_within_range(ivec3* chunk_pos, int x, int y, int z) {
    return abs(chunk_pos->x - x) <= CHUNK_RENDER_DIST &&
            abs(chunk_pos->y - y) <= CHUNK_RENDER_DIST &&
            abs(chunk_pos->z - z) <= CHUNK_RENDER_DIST;
}

void RenderableSuperObject::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    float x_offset = fmodf((fmodf(new_pos->x, CX) + CX), CX);
    float y_offset = fmodf((fmodf(new_pos->y, CY) + CY), CY);
    float z_offset = fmodf((fmodf(new_pos->z, CZ) + CZ), CZ);
    ivec3 new_chunk_pos = ivec3((new_pos->x - x_offset) / CX, (new_pos->y - y_offset) / CY, (new_pos->z - z_offset) / CZ);
    int xmin, xmax, ymin, ymax, zmin, zmax;
    ivec3 old_chunk_pos;
    bool check_old = true;
    if (old_pos) {
        // we don't need to load chunks we already have loaded in
        x_offset = fmodf((fmodf(old_pos->x, CX) + CX), CX);
        y_offset = fmodf((fmodf(old_pos->y, CY) + CY), CY);
        z_offset = fmodf((fmodf(old_pos->z, CZ) + CZ), CZ);
        old_chunk_pos = ivec3((old_pos->x - x_offset) / CX, (old_pos->y - y_offset) / CY, (old_pos->z - z_offset) / CZ);
        
        if (old_chunk_pos == new_chunk_pos) {
            return;
        }
        
        xmin = minimum(old_chunk_pos.x, new_chunk_pos.x) - CHUNK_RENDER_DIST;
        xmax = maximum(old_chunk_pos.x, new_chunk_pos.x) + CHUNK_RENDER_DIST + 1;
        ymin = minimum(old_chunk_pos.y, new_chunk_pos.y) - CHUNK_RENDER_DIST;
        ymax = maximum(old_chunk_pos.y, new_chunk_pos.y) + CHUNK_RENDER_DIST + 1;
        zmin = minimum(old_chunk_pos.z, new_chunk_pos.z) - CHUNK_RENDER_DIST;
        zmax = maximum(old_chunk_pos.z, new_chunk_pos.z) + CHUNK_RENDER_DIST + 1;
    }
    else {
        // if this chunk has never been updated before, all chunks must be new
        xmin = new_chunk_pos.x - CHUNK_RENDER_DIST;
        xmax = new_chunk_pos.x + CHUNK_RENDER_DIST + 1;
        ymin = new_chunk_pos.y - CHUNK_RENDER_DIST;
        ymax = new_chunk_pos.y + CHUNK_RENDER_DIST + 1;
        zmin = new_chunk_pos.z - CHUNK_RENDER_DIST;
        zmax = new_chunk_pos.z + CHUNK_RENDER_DIST + 1;
        check_old = false;
    }

    bool within_old, within_new;
    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            for (int z = zmin; z < zmax; z++) {
                within_old = check_old && is_within_range(&old_chunk_pos, x, y, z);
                within_new = is_within_range(&new_chunk_pos, x, y, z);
                if (within_old && !within_new) {
                    delete_chunk(x, y, z);
                }
                else if (!within_old && within_new) {
                    load_chunk(x, y, z);
                }
            }
        }
    }
    

}