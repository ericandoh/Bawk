//
//  directionallightrender.h
//  Bawk
//
//  Created by Eric Oh on 12/24/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__directionallightrender__
#define __Bawk__directionallightrender__

#include <stdio.h>
#include "basic_types.h"

class DirectionalRenderableLight {
    fmat4 mvp;
public:
    float light_intensity;
    // direction from player to light
    fvec3 direction;
    DirectionalRenderableLight();
    void render_light();
    void set_camera(fvec3 player_pos);
    void set_camera_for_light(fvec3 player_pos);
};

#endif /* defined(__Bawk__directionallightrender__) */
