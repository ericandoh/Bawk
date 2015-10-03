//
//  superobjectrender.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "superobjectrender.h"
#include <glm/gtc/matrix_transform.hpp>
#include "worldrender.h"

RenderableSuperObject::~RenderableSuperObject() {
    for (auto iterator = chunks.begin(); iterator != chunks.end(); iterator++) {
        delete iterator->second;
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

uint16_t RenderableSuperObject::get(int x, int y, int z) {
    ivec3 pos = ivec3(x / CX, y / CY, z / CZ);
    int x_offset = x % CX;
    int y_offset = y % CY;
    int z_offset = z % CZ;
    
    if (chunks.count(pos)) {
        // the superobject has the object within its range
        return chunks[pos]->get(x_offset, y_offset, z_offset);
    }
    // handle if not within chunk boundary!!!
    // this isn't in memory, so we would do some sort of lookup here...or just fail it
    // (it's out of bounds!)
    // TODO
    return 0;
}

void RenderableSuperObject::set(int x, int y, int z, uint16_t type) {
    ivec3 pos = ivec3(x / CX, y / CY, z / CZ);
    int x_offset = x % CX;
    int y_offset = y % CY;
    int z_offset = z % CZ;
    
    if (chunks.count(pos)) {
        // the superobject has the object within its range
        chunks[pos]->set(x_offset, y_offset, z_offset, type);
    }
    // handle if not within chunk boundary!!!
    // this isn't in memory, so we would do some sort of lookup here...or just fail it
    // (it's out of bounds!)
    // TODO
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