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
// used for placement relative to
BlockOrientation face;
// is the player's orientation, set as orientation of block when placed
BlockOrientation player_face;
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
    
    // do voxel block tracing
    WorldTracer tracer(world);
    tracer.bresenham3D(start.x, start.y, start.z, end.x, end.y, end.z);
    
    // get direction player is facing
    player_face = get_nearest_compass_direction(dir);
}

bool get_look_at(ivec4* src) {
    if (in_range) { // & world_selected
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

bool get_pointing_position(ivec3* dst, BlockOrientation* orient, ivec3 bounds) {
    ivec4 looking_at;
    if (!get_look_at(&looking_at)) {
        return false;
    }
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    BlockOrientation face = static_cast<BlockOrientation>(looking_at.w);
    if(face == BlockOrientation::FRONT)
        mx++;
    if(face == BlockOrientation::BACK)
        mx-=bounds.x;
    if(face == BlockOrientation::TOP)
        my++;
    if(face == BlockOrientation::BOTTOM)
        my-=bounds.y;
    if(face == BlockOrientation::LEFT)
        mz++;
    if(face == BlockOrientation::RIGHT)
        mz-=bounds.z;
    dst->x = mx;
    dst->y = my;
    dst->z = mz;
    *orient = face;
    return true;
}

Entity* get_look_at() {
    return selected;
}
