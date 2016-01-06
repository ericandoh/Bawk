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


namespace BlockTracing {
    // exposed variables
    bool show_item = false;
    Entity* selected = 0;
    fvec3 pointed_pos;
    ivec3 pointed_rounded_pos;
    
    // used for placement relative to
    BlockOrientation face;
    // is the player's orientation, set as orientation of block when placed
    BlockOrientation player_face;
}

using namespace BlockTracing;

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
    world->get_entity_at(x, y, z, &selected);
    if (selected) {
        pointed_pos.x = x;
        pointed_pos.y = y;
        pointed_pos.z = z;
        
        pointed_rounded_pos.x = floorf(pointed_pos.x);
        pointed_rounded_pos.y = floorf(pointed_pos.y);
        pointed_rounded_pos.z = floorf(pointed_pos.z);
        face = side;
        show_item = true;
        return true;
    }
    return false;
}

void set_look_at(fvec3 pos, fvec3 dir, World* world) {
    
    // let's use ray casting instead!
    // algorithm: Bresenham's
    fvec3 start(pos.x + dir.x*PLACE_BLOCK_FRONT, pos.y + dir.y*PLACE_BLOCK_FRONT, pos.z + dir.z*PLACE_BLOCK_FRONT);
    fvec3 end(pos.x + dir.x*PLACE_BLOCK_BACK, pos.y + dir.y*PLACE_BLOCK_BACK, pos.z + dir.z*PLACE_BLOCK_BACK);
    
    selected = 0;
    show_item = false;
    
    // do voxel block tracing
    WorldTracer tracer(world);
    tracer.bresenham3D(start.x, start.y, start.z, end.x, end.y, end.z);
    
    // get direction player is facing
    player_face = get_nearest_compass_direction(dir);
}

bool get_pointing_position(ivec3* dst, BlockOrientation* orient, ivec3 bounds) {
    if (!show_item) {
        return false;
    }
    ivec3 looking_at = pointed_rounded_pos;
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
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

BlockOrientation get_player_direction() {
    return player_face;
}

