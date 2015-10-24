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

class World;

void set_look_at(fvec3 pos, fvec3 dir, float depth, fvec3 objcoord, World* world);
bool get_look_at(ivec4* src);
Entity* get_look_at();

#endif /* defined(__Bawk__blocktracer__) */
