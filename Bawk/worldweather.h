//
//  worldweather.h
//  Bawk
//
//  Created by Eric Oh on 12/17/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__worldweather__
#define __Bawk__worldweather__

#include <stdio.h>
#include <vector>
#include "basic_types.h"
#include "spriterender.h"
#include "directionallightrender.h"

class Player;

class WorldWeather {
    // TODO finish this up
    // colors for daytime/nighttime/sunttime
    fvec3 daytime, nighttime, suntime;
    int xfragments, yfragments;
    int distance;
    // TODO we can't do this, not exactly a skybox...
    std::vector<std::vector<SpriteRender*>> background_sprites;
    
    // keep track of time, or make this a pointer of some sort
    int time;
    int time_per_day;
    int time_update;
    
public:
    DirectionalRenderableLight dir_light;
    
    WorldWeather();
    void setup();
    void render(fmat4* transform, Player* player);
    void step();
    void update_sky();
};

#endif /* defined(__Bawk__worldweather__) */
