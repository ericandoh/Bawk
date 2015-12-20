//
//  spritecolortriangle.h
//  Bawk
//
//  Created by Eric Oh on 12/20/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__spritecolortriangle__
#define __Bawk__spritecolortriangle__

#include <stdio.h>
#include "spriterender.h"

class SpriteColorTriangle: public SpriteRenderable {
public:
    // color for the 4 corners
    // lower left, lower right, upper left, upper right
    fvec3 colors[3];
    // relative to player at position 0,0,0
    fvec3 corners[3];
    
    SpriteColorTriangle();
    void render(fmat4* transform, SpriteRender* render) override;
    SpriteRenderable* copy() override;
};

#endif /* defined(__Bawk__spritecolortriangle__) */
