//
//  spritemanager.h
//  Bawk
//
//  Note: this does NOT hold all sprites. some sprites may be held by other entities and those
//  entities will be responsible for managing the render/step of the respective sprite
//  This manager just provides a convenient way to render sprites without having to worry
//  about managing them once they have been made
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__spritemanager__
#define __Bawk__spritemanager__

#include <stdio.h>
#include <vector>
#include "spriterender.h"

class Game;

class SpriteManager {
    std::vector<SpriteRender*> sprites;
public:
    SpriteManager();
    
    void render(fmat4* transform);
    void render_lights(fmat4* transform, fvec3 player_pos);
    void step(Game* game, int ms);
    
    void add_sprite(SpriteRender* sprite);
    void remove_sprite(SpriteRender* sprite);
};

#endif /* defined(__Bawk__spritemanager__) */
