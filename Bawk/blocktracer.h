//
//  blocktracer.h
//  Bawk
//
//  Created by Eric Oh on 10/24/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__blocktracer__
#define __Bawk__blocktracer__

#include <stdio.h>
#include "entity.h"
#include "block_orientation.h"

class World;

namespace BlockTracing {
    extern bool show_item;
    extern Entity* selected;
    extern fvec3 pointed_pos;
    extern ivec3 pointed_rounded_pos;
}

void set_look_at(fvec3 pos, fvec3 dir, World* world);

bool get_pointing_position(ivec3* dst, BlockOrientation* orient, ivec3 bounds);
BlockOrientation get_player_direction();

#endif /* defined(__Bawk__blocktracer__) */
