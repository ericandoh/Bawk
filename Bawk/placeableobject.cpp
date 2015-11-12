//
//  placeableobject.cpp
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "placeableobject.h"
#include "world.h"
#include "gametemplate.h"
#include <glm/gtc/matrix_transform.hpp>

PlaceableObject::PlaceableObject() {
    mvp_set = false;
}

void PlaceableObject::set_mvp(ivec3 bounds) {
    float length = sqrtf(bounds.x*bounds.x + bounds.y*bounds.y + bounds.z*bounds.z) / 1.4f;
    
    fvec3 v1 = fvec3(-bounds.x, 0, bounds.z);
    fvec3 v2 = fvec3(0, -bounds.y, bounds.z);
    fvec3 perp = glm::cross(v1, v2);
    // normalize perp
    fvec3 perp_normalized = glm::normalize(perp);
    perp.x = perp_normalized.x * length * 2.0f;
    perp.y = perp_normalized.y * length * 2.0f;
    perp.z = perp_normalized.z * length * 2.0f;
    
    fvec3 upper_pos = fvec3(perp.x*2.0f, perp.y*2.0f, perp.z*2.0f);
    fvec3 toward_pos = fvec3(bounds.x/2.0f, bounds.y/2.0f, bounds.z/2.0f);
    
    fmat4 view = glm::lookAt(upper_pos, toward_pos, fvec3(0, 1, 0));
    //fmat4 projection = glm::perspective(45.0f, 1.0f, 0.01f, 100.0f); //1.0f * width/height
    
    // left, right, bottom, top
    fmat4 projection = glm::ortho(-length, length, -length, length, 0.01f, 100.0f);
    mvp = projection * view;
    mvp_set = true;
}

bool PlaceableObject::set_blocks(Player* player, World* world, GameTemplate* temp) {
    if (temp) {
        // we want to place these blocks into temp, and temp only
    }
    else {
        // place blocks into the world
        if (set_blocks(world, world->base_world, true)) {
            set_blocks(world, temp, false);
            return true;
        }
    }
    
    return false;
}