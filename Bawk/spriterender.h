//
//  spriterender.h
//  Bawk
//
//  Created by Eric Oh on 12/8/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__spriterender__
#define __Bawk__spriterender__

#include <stdio.h>
#include "entity.h"

class SpriteRender: public Entity {
public:
    // time to live, set to -1 if this will keep living
    unsigned long ttl;
    SpriteRender();
    
    // --- Entity ---
    void step() override;
    void render(fmat4* transform) override;
    void render_lights(fmat4* transform, fvec3 player_pos);

};

#endif /* defined(__Bawk__spriterender__) */
