//
//  Superobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "superobject.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "chunk.h"
#include "worldrender.h"

void SuperObject::init() {
    // make three random chunks at different offsets
    chunks[ivec3(0, 1, 0)] = Chunk();
    chunks[ivec3(1, 2, 3)] = Chunk();
    chunks[ivec3(0, 0, 2)] = Chunk();
    for (auto& iterator : chunks) {
        ivec3 pos = iterator.first;
        printf("Chunk at %d,%d,%d\n", pos.x, pos.y, pos.z);
    }
    printf("Map size %d\n", chunks.size());
}

void SuperObject::remove() {
    for (auto iterator = chunks.begin(); iterator != chunks.end(); iterator++) {
        iterator->second.remove();
    }
}

uint8_t SuperObject::get(int x, int y, int z) {
    ivec3 pos = ivec3(x / CX, y / CY, z / CZ);
    int x_offset = x % CX;
    int y_offset = y % CY;
    int z_offset = z % CZ;
    
    if (chunks.count(pos)) {
        // the superobject has the object within its range
        return chunks[pos].get(x_offset, y_offset, z_offset);
    }
    // handle if not within chunk boundary!!!
    // this isn't in memory, so we would do some sort of lookup here...or just fail it
    // (it's out of bounds!)
    // TODO
    return 0;
}

void SuperObject::set(int x, int y, int z, uint8_t type) {
    ivec3 pos = ivec3(x / CX, y / CY, z / CZ);
    int x_offset = x % CX;
    int y_offset = y % CY;
    int z_offset = z % CZ;
    
    if (chunks.count(pos)) {
        // the superobject has the object within its range
        chunks[pos].set(x_offset, y_offset, z_offset, type);
    }
    // handle if not within chunk boundary!!!
    // this isn't in memory, so we would do some sort of lookup here...or just fail it
    // (it's out of bounds!)
    // TODO
}

void SuperObject::render(fmat4* transform) {
    for (auto iterator = chunks.begin(); iterator != chunks.end(); iterator++) {
        ivec3 pos = iterator->first;
        
        fmat4 model = glm::translate(fmat4(1.0f),
                                     fvec3(pos.x * CX,
                                           pos.y * CY,
                                           pos.z * CZ));
        fmat4 mvp = *transform * model;
        
        glUniformMatrix4fv(block_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
        
        iterator->second.render();
    }
}