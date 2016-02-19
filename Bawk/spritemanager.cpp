//
//  spritemanager.cpp
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "spritemanager.h"

SpriteManager::SpriteManager() {
    
}

void SpriteManager::render(fmat4* transform) {
    for (auto &i: sprites) {
        i->render(transform);
    }
}

void SpriteManager::render_lights(fmat4* transform, fvec3 player_pos) {
    for (auto &i: sprites) {
        i->render_lights(transform, player_pos);
    }
}

void SpriteManager::step(int ms) {
    for (auto &i: sprites) {
        i->step(ms);
    }
}

void SpriteManager::add_sprite(SpriteRender* sprite) {
    sprites.push_back(sprite);
}

void SpriteManager::remove_sprite(SpriteRender* sprite) {
    // TODO do this more efficiently, this will not scale
    for (unsigned int i = 0; i < sprites.size(); i++) {
        if (sprites[i] == sprite) {
            sprites.erase(sprites.begin() + i);
            break;
        }
    }
    // TODO sprites are leaking memory...and a simple delete won't do!
    sprite->multiplexer = 0;
    // delete sprite;
}