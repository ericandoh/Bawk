//
//  spritemodel.h
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__spritemodel__
#define __Bawk__spritemodel__

#include <stdio.h>
#include "spriterender.h"
#include "modeldata.h"

class SpriteModel: public SpriteMultiplexer {
public:
    ModelData* model;
    SpriteModel();
    void render(fmat4* transform, SpriteRender* render) override;
};

// constructs an explosive sprite given the parameters
SpriteMultiplexer* get_sprite_model(Json::Value node);

#endif /* defined(__Bawk__spritemodel__) */
