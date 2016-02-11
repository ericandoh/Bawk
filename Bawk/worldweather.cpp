//
//  worldweather.cpp
//  Bawk
//
//  Created by Eric Oh on 12/17/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "worldweather.h"
#include "spritegetter.h"

WorldWeather::WorldWeather() {
    radius = 450;
    
    time = 0;
    time_per_day = 24000;
    
    time_update = 5;
    
    dir_light = DirectionalRenderableLight();
    skybox = get_sprite_instance(3);
    skybox->set_track_player(true);
    skybox->set_pos(fvec3(0,0,0));
}

void WorldWeather::render(fmat4* transform) {
    // bind the skybox texture
    
    // scale the skybox
    fmat4 view;
    view = glm::scale(*transform, fvec3(radius, radius, radius));
    skybox->render(&view);
    // bind back to default texture
    
}

void WorldWeather::step() {
    time++;
    if (time > time_per_day) {
        time = time - time_per_day;
    }
    if (time % time_update == 0) {
        update_sky();
    }
}

float get_min_dist(float x, float y, float mod) {
    float val1 = fmodf(fmodf(x - y, mod) + mod, mod);
    float val2 = fmodf(fmodf(y - x, mod) + mod, mod);
    return std::min(val1, val2);
}

void WorldWeather::update_sky() {
    float frac_cycle = time * 1.0f / time_per_day;
    
    float frac_day = 0.5f + 0.5f * cosf(frac_cycle * 2 * M_PI);
    float frac_night = 0.5f + 0.5f * sinf(frac_cycle * 2 * M_PI);
    
    Rotation* tilt = skybox->get_angle();
    tilt->set_angle(glm::angleAxis((float)(frac_cycle * 2 * M_PI), fvec3(0,0,1)));
    
    dir_light.direction = fvec3(0.5f - frac_day, 0.4f, 0.2f);
    dir_light.direction = glm::normalize(dir_light.direction);
    
    float sun_x;
    if (frac_cycle < 0.5f) {
        // daytime
        sun_x = 0.5f;
    }
    else {
        sun_x = 0.0f;
    }
}