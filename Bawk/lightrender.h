//
//  lightrender.h
//  Bawk
//
//  Created by Eric Oh on 12/3/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__lightrender__
#define __Bawk__lightrender__

#include <stdio.h>
#include "basic_types.h"

class RenderableLight {
public:
    float light_radius;
    float light_intensity;
    RenderableLight();
    void render_light(fvec3 rwc, fvec3 player_pos);
    void render_light(fmat4* transform, fvec3 p, fvec3 player_pos);
    bool should_render();
};

#endif /* defined(__Bawk__lightrender__) */
