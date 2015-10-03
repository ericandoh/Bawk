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

void RenderableSuperObject::randomize() {
    // make three random chunks at different offsets
    chunks[ivec3(0, 1, 0)] = new RenderableChunk();
    chunks[ivec3(1, 2, 3)] = new RenderableChunk();
    chunks[ivec3(0, 0, 2)] = new RenderableChunk();
    for (auto& iterator : chunks) {
        ivec3 pos = iterator.first;
        printf("Chunk at %d,%d,%d\n", pos.x, pos.y, pos.z);
    }
}

int RenderableSuperObject::load_chunk(int x, int y, int z) {
    printf("Loading chunk for %d %d %d\n", x, y, z);
    uint16_t raw_chunk[CX][CY][CZ];
    if (get_chunk(raw_chunk, x, y, z)) {
        // failed to load chunk
        return 1;
    }
    chunks[ivec3(x, y, z)] = new RenderableChunk(raw_chunk);
    return 0;
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
        set_transform_matrix(mvp);
        iterator->second->render();
    }
}