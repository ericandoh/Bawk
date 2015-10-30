//
//  blocktracer.cpp
//  Bawk
//
//  Created by Eric Oh on 10/24/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "blocktracer.h"
#include "bresenham3d.h"
#include "world.h"

const float PLACE_BLOCK_FRONT = 1.4f;
const float PLACE_BLOCK_BACK = 10.0f;

float mx, my, mz = 0;
BlockOrientation face;
bool in_range = false;
Entity* selected = 0;
bool world_selected = false;

class WorldTracer: public BresenhamTracer {
    World* world;
public:
    WorldTracer(World* w);
    bool check_at_coord(float x, float y, float z, BlockOrientation side) override;
};

/*static float dti(float val) {
    return fabsf(val - roundf(val));
}*/

WorldTracer::WorldTracer(World* w) {
    world = w;
}

bool WorldTracer::check_at_coord(float x, float y, float z, BlockOrientation side) {
    world->get_at(x, y, z, &world_selected, &selected);
    if (world_selected || selected) {
        mx = x;
        my = y;
        mz = z;
        face = side;
        in_range = true;
        return true;
    }
    /*if (world->get_block(x, y, z).type) {
        mx = floorf(x);
        my = floorf(y);
        mz = floorf(z);
        world_selected = true;
        selected = 0;
        face = side;
        in_range = true;
        return true;
    }*/
    return false;
}

void set_look_at(fvec3 pos, fvec3 dir, World* world) {
    
    // let's use ray casting instead!
    // algorithm: Bresenham's
    fvec3 start(pos.x + dir.x*PLACE_BLOCK_FRONT, pos.y + dir.y*PLACE_BLOCK_FRONT, pos.z + dir.z*PLACE_BLOCK_FRONT);
    fvec3 end(pos.x + dir.x*PLACE_BLOCK_BACK, pos.y + dir.y*PLACE_BLOCK_BACK, pos.z + dir.z*PLACE_BLOCK_BACK);
    
    world_selected = false;
    selected = 0;
    in_range = false;
    
    WorldTracer tracer(world);
    tracer.bresenham3D(start.x, start.y, start.z, end.x, end.y, end.z);
    /*
    // old depth-buffer method, which doesn't work for model objects
    mx = floorf(objcoord.x);
    my = floorf(objcoord.y);
    mz = floorf(objcoord.z);
    
    float distance = sqrtf((mx-pos.x)*(mx-pos.x) + (my-pos.y)*(my-pos.y) + (mz-pos.z)*(mz-pos.z));
    
    if(dti(objcoord.x) < dti(objcoord.y)) {
        if(dti(objcoord.x) < dti(objcoord.z)) {
            // face is perpendicular to x-axis (most likely)
            if(dir.x > 0) {
                face = BlockOrientation::BACK;
            } else {
                face = BlockOrientation::FRONT;
            }
        } else {
            // face is perpendicular to z-axis (most likely)
            if(dir.z > 0) {
                face = BlockOrientation::LEFT;
            } else {
                face = BlockOrientation::RIGHT;
            }
        }
    } else {
        if(dti(objcoord.y) < dti(objcoord.z)) {
            // face is perpendicular to y-axis (most likely)
            if(dir.y > 0) {
                face = BlockOrientation::BOTTOM;
            } else {
                face = BlockOrientation::TOP;
            }
        } else {
            // face is perpendicular to z-axis (most likely)
            if(dir.z > 0) {
                face = BlockOrientation::LEFT;
            } else {
                face = BlockOrientation::RIGHT;
            }
        }
    }*/
}

bool get_look_at(ivec4* src) {
    if (in_range && world_selected) {
        src->x = floorf(mx);
        src->y = floorf(my);
        src->z = floorf(mz);
        src->w = face;
        return true;
    }
    return false;
}

bool get_look_at_vehicle(fvec4* src) {
    if (in_range && selected) {
        src->x = mx;
        src->y = my;
        src->z = mz;
        src->w = face;
        return true;
    }
    return false;
}

Entity* get_look_at() {
    return selected;
}
