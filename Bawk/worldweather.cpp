//
//  worldweather.cpp
//  Bawk
//
//  Created by Eric Oh on 12/17/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "worldweather.h"
#include "spritecolorquad.h"
#include "spritecolortriangle.h"

#define SKYBOX_XFRAGS 10
#define SKYBOX_YFRAGS 20

WorldWeather::WorldWeather() {
    distance = 500;
    
    daytime = fvec3(0.53f, 0.808f, 0.98f);
    nighttime = fvec3(0.05f, 0.01f, 0.09f);
    suntime = fvec3(0.93f, 0.94f, 0.02f);
    
    time = 0;
    time_per_day = 24000;
    
    time_update = 25;
    
    setup();
}

void WorldWeather::setup() {
    fvec3 all_corners[SKYBOX_YFRAGS + 1][SKYBOX_XFRAGS];
    for (int y = 0; y < SKYBOX_YFRAGS + 1; y++) {
        float yangle = 2 * M_PI * (y) / SKYBOX_YFRAGS;
        float ypos = cosf(yangle) * distance;
        for (int x = 0; x < SKYBOX_XFRAGS; x++) {
            float xangle = 2 * M_PI * x / SKYBOX_XFRAGS;
            float xpos = cosf(xangle) * sinf(yangle) * distance;
            float zpos = sinf(xangle) * sinf(yangle) * distance;
            all_corners[y][x] = fvec3(xpos, ypos, zpos);
        }
    }
    
    background_sprites.resize(SKYBOX_YFRAGS);
    for (int y = 0; y < SKYBOX_YFRAGS; y++) {
        background_sprites[y].resize(SKYBOX_XFRAGS);
        if (y == 0 || y == SKYBOX_YFRAGS - 1) {
            // special case - we have triangles instead of quads
            for (int x = 0; x < SKYBOX_XFRAGS; x++) {
                int xe = (x + 1) % SKYBOX_XFRAGS;
                int ye = y + 1;
                SpriteColorTriangle* tri = new SpriteColorTriangle();
                tri->corners[0] = all_corners[y][x];
                tri->corners[1] = all_corners[y][xe];
                tri->corners[2] = all_corners[ye][x];
                
                for (int i = 0; i < 3; i++) {
                    tri->colors[i] = fvec3(0.0f, 0.0f, 0.0f);
                }
                
                SpriteRender* sprite = new SpriteRender();
                sprite->can_rotate = false;
                sprite->renderable = tri;
                background_sprites[y][x] = sprite;
            }
        }
        else {
            for (int x = 0; x < SKYBOX_XFRAGS; x++) {
                int xe = (x + 1) % SKYBOX_XFRAGS;
                int ye = y + 1;
                
                SpriteColorQuad* quad = new SpriteColorQuad();
                quad->corners[0] = all_corners[y][x];
                quad->corners[1] = all_corners[y][xe];
                quad->corners[2] = all_corners[ye][x];
                quad->corners[3] = all_corners[ye][xe];
                
                for (int i = 0; i < 4; i++) {
                    quad->colors[i] = fvec3(0.0f, 0.0f, 0.0f);
                }
                
                SpriteRender* sprite = new SpriteRender();
                sprite->can_rotate = false;
                sprite->renderable = quad;
                background_sprites[y][x] = sprite;
            }
        }
    }
}

void WorldWeather::render(fmat4* transform, Player* player) {
    for (int y = 0; y < SKYBOX_YFRAGS; y++) {
        for (int x = 0; x < SKYBOX_XFRAGS; x++) {
            background_sprites[y][x]->player = player;
            background_sprites[y][x]->render(transform);
        }
    }
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
    
    float sun_x;
    if (frac_cycle < 0.5f) {
        // daytime
        sun_x = 0.5f;
    }
    else {
        sun_x = 0.0f;
    }
    
    fvec3 all_colors[SKYBOX_YFRAGS + 1][SKYBOX_XFRAGS];
    for (int y = 0; y < SKYBOX_YFRAGS + 1; y++) {
        //yangle = 2 * M_PI * (y) / SKYBOX_YFRAGS;
        for (int x = 0; x < SKYBOX_XFRAGS; x++) {
            // distance from x, y
            //xangle = 2 * M_PI * x / SKYBOX_XFRAGS;
            float frac_sun_x = get_min_dist(x * 1.0f / SKYBOX_XFRAGS, sun_x, 1.0f) * 2.0f;
            float frac_sun_y = get_min_dist(y * 1.0f / SKYBOX_YFRAGS, frac_day, 1.0f) * 2.0f;
            float frac_sun = (frac_sun_x + frac_sun_y) / 2.0f;
            frac_sun = 1.0f - frac_sun;
            frac_sun = (frac_sun * frac_sun * frac_sun);
            //frac_sun = 0.0f;
            //frac_night = 1.0f;
            //frac_day = 0.0f;
            float total_sum = frac_sun + frac_day + frac_night;
            //float xpos = cosf(xangle) * sinf(yangle) * distance;
            //float zpos = sinf(xangle) * sinf(yangle) * distance;
            all_colors[y][x] = ((frac_day * daytime) + (frac_night * nighttime) + (frac_sun * suntime)) / total_sum;
        }
    }
    
    for (int y = 0; y < SKYBOX_YFRAGS; y++) {
        bool is_triangle = false;
        if (y == 0 || y == SKYBOX_YFRAGS - 1) {
            // we have a triangle
            is_triangle = true;
        }
        for (int x = 0; x < SKYBOX_XFRAGS; x++) {
            int xe = (x + 1) % SKYBOX_XFRAGS;
            int ye = y + 1;
            if (is_triangle) {
                SpriteColorTriangle* tri = (SpriteColorTriangle*)background_sprites[y][x]->renderable;
                if (y == 0) {
                    tri->colors[0] = all_colors[y][0];
                    tri->colors[1] = all_colors[y][0];
                    tri->colors[2] = all_colors[ye][x];
                }
                else {
                    tri->colors[0] = all_colors[y][x];
                    tri->colors[1] = all_colors[y][xe];
                    tri->colors[2] = all_colors[ye][0];
                }
            }
            else {
                SpriteColorQuad* quad = (SpriteColorQuad*)background_sprites[y][x]->renderable;
                quad->colors[0] = all_colors[y][x];
                quad->colors[1] = all_colors[y][xe];
                quad->colors[2] = all_colors[ye][x];
                quad->colors[3] = all_colors[ye][xe];
            }
        }
    }
}